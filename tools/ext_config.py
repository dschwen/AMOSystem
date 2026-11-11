#!/usr/bin/env python3
"""The interpreter's configuration with extensions in slots:

  ext_config.py AMOSPro_Interpreter_Config OUT [SLOT=FILE ...]

The extensions' file names are the configuration's messages 16 to 41 (slot 1 to 26: a length
byte, the name, a 0). Without SLOT=FILE, the distribution's: AMCAF in slot 8 and Turbo Plus in
slot 12 (their default slots; docs/THIRD-PARTY-EXTENSIONS.md). An empty FILE frees the slot.
Nothing else changes.
"""
import struct
import sys

DEFAULT = ['8=AMOSPro_AMCAF.Lib', '12=AMOSPro_TURBO_Plus.Lib']
FIRST = 16                                  # the message of slot 1


def assign(data, slots):
    if data[:4] != b'PId1':
        raise ValueError('not an interpreter configuration')
    values = struct.unpack_from('>I', data, 4)[0]
    at = 8 + values
    if data[at:at + 4] != b'PIt1':
        raise ValueError('no texts')
    size = struct.unpack_from('>I', data, at + 4)[0]
    texts = data[at + 8:at + 8 + size]
    messages, i = [], 1
    while texts[i] != 0xFF:
        n = texts[i]
        messages.append(texts[i + 1:i + 1 + n])
        i += 2 + n
    tail = texts[i:]                        # (the end mark, and what follows)
    for slot, file in slots.items():
        if not 1 <= slot <= 26:
            raise ValueError(f'slot {slot}: 1 to 26')
        messages[FIRST + slot - 1 - 1] = file.encode()
    out = bytearray(texts[:1])
    for m in messages:
        out += bytes([len(m)]) + m + b'\0'
    out += tail
    return data[:at + 4] + struct.pack('>I', len(out)) + bytes(out) + data[at + 8 + size:]


def patch_file(path, args=None):
    """The configuration at path with the distribution's extensions (the packagers'). A file that
    is not there is not an error (a staging tree without it)."""
    from pathlib import Path
    path = Path(path)
    if path.exists():
        path.write_bytes(assign(path.read_bytes(), parse(args)))


def parse(args):
    slots = {}
    for a in args or DEFAULT:
        slot, _, file = a.partition('=')
        slots[int(slot)] = file
    return slots


if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    with open(sys.argv[1], 'rb') as f:
        data = f.read()
    with open(sys.argv[2], 'wb') as f:
        f.write(assign(data, parse(sys.argv[3:])))
