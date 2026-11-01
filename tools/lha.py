#!/usr/bin/env python3
"""A small LHA (.lha, .lzh) extractor, in Python only.

  lha.py ARCHIVE [-C DIRECTORY] [PATTERN...]   extract (the whole archive, or the
                                              files whose path contains a pattern)
  lha.py -l ARCHIVE                           list

For the NDK's archive (tools/fetch_ndk.py) where no LHA tool is at hand. It
reads the headers of levels 0, 1 and 2, the methods -lh0- (stored), -lh5-,
-lh6- and -lh7- (LZSS with static Huffman codes), and checks each file's
CRC-16. Not a general tool: no -lh1-..-lh4-, no -lzs-, no -lhd- entries'
attributes, no write.
"""

import argparse
import os
import struct
import sys

THRESHOLD, MAXMATCH, NT, TBIT, CBIT = 3, 256, 19, 5, 9
NC = 255 + MAXMATCH - THRESHOLD + 1

_CRC = []
for _i in range(256):
    _c = _i
    for _ in range(8):
        _c = (_c >> 1) ^ 0xA001 if _c & 1 else _c >> 1
    _CRC.append(_c)


def crc16(data, crc=0):
    for b in data:
        crc = (crc >> 8) ^ _CRC[(crc ^ b) & 0xFF]
    return crc


class Bits:
    """The bits of a string of bytes, the first of a byte the high one"""

    def __init__(self, data):
        self.data, self.pos, self.buf, self.count = data, 0, 0, 0

    def get(self, n):
        while self.count < n:
            self.buf = (self.buf << 8) | (self.data[self.pos] if self.pos < len(self.data) else 0)
            self.pos += 1
            self.count += 8
        self.count -= n
        v = (self.buf >> self.count) & ((1 << n) - 1)
        self.buf &= (1 << self.count) - 1
        return v

    def peek(self, n):
        while self.count < n:
            self.buf = (self.buf << 8) | (self.data[self.pos] if self.pos < len(self.data) else 0)
            self.pos += 1
            self.count += 8
        return (self.buf >> (self.count - n)) & ((1 << n) - 1)


def canonical(lengths):
    """Decoding table of a canonical Huffman code: {(length, code): symbol},
    and the longest length (LHA gives the codes by length, in the order of
    the symbols)"""
    table, code, longest = {}, 0, max(lengths) if lengths else 0
    for length in range(1, longest + 1):
        for symbol, l in enumerate(lengths):
            if l == length:
                table[(length, code)] = symbol
                code += 1
        code <<= 1
    return table, longest


class Huffman:
    def __init__(self, lengths=None, constant=None):
        self.constant = constant
        if constant is None:
            self.table, self.longest = canonical(lengths)

    def read(self, bits):
        if self.constant is not None:
            return self.constant
        code = 0
        for length in range(1, self.longest + 1):
            code = (code << 1) | bits.get(1)
            symbol = self.table.get((length, code))
            if symbol is not None:
                return symbol
        raise ValueError('bad Huffman code')


def read_lengths(bits, count_bits, special, size, pt=None):
    """The code lengths of the table of the symbols' lengths (pt None) or of
    the offsets, as LHA writes them"""
    n = bits.get(count_bits)
    if n == 0:
        return None, bits.get(count_bits)
    lengths = []
    while len(lengths) < n:
        c = bits.get(3)
        if c == 7:
            while bits.get(1):
                c += 1
        lengths.append(c)
        if len(lengths) == special:
            lengths.extend([0] * bits.get(2))
    return lengths + [0] * (size - len(lengths)), None


def decode_lh(data, method, size):
    dic_bits = {5: 13, 6: 15, 7: 16}[method]
    np_, pbit = dic_bits + 1, 4 if method == 5 else 5
    bits, out = Bits(data), bytearray()
    while len(out) < size:
        block = bits.get(16)
        # the code of the lengths
        lengths, constant = read_lengths(bits, TBIT, 3, NT)
        pt = Huffman(constant=constant) if lengths is None else Huffman(lengths)
        # the code of the characters and lengths of matches
        n = bits.get(CBIT)
        if n == 0:
            c_code = Huffman(constant=bits.get(CBIT))
        else:
            c_len = []
            while len(c_len) < n:
                c = pt.read(bits)
                if c <= 2:
                    c = 1 if c == 0 else bits.get(4) + 3 if c == 1 else bits.get(CBIT) + 20
                    c_len.extend([0] * c)
                else:
                    c_len.append(c - 2)
            c_code = Huffman(c_len + [0] * (NC - len(c_len)))
        # the code of the offsets
        lengths, constant = read_lengths(bits, pbit, -1, np_)
        p_code = Huffman(constant=constant) if lengths is None else Huffman(lengths)
        for _ in range(block):
            if len(out) >= size:
                break
            c = c_code.read(bits)
            if c < 256:
                out.append(c)
                continue
            length = c - 256 + THRESHOLD
            j = p_code.read(bits)
            offset = j if j < 2 else (1 << (j - 1)) + bits.get(j - 1)
            for _ in range(length):
                out.append(out[-offset - 1])
    return bytes(out[:size])


def entries(data):
    """(path, method, packed bytes, original size, crc) of each file"""
    pos = 0
    while pos < len(data) and data[pos] != 0:
        level = data[pos + 20]
        method = data[pos + 2:pos + 7].decode('latin-1')
        packed, size = struct.unpack_from('<II', data, pos + 7)
        name, directory, skipped = '', '', 0
        if level == 0:
            n = data[pos + 21]
            name = data[pos + 22:pos + 22 + n].decode('latin-1')
            crc = struct.unpack_from('<H', data, pos + 22 + n)[0]
            start = pos + data[pos] + 2
        else:
            if level == 1:
                base = data[pos] + 2
                n = data[pos + 21]
                name = data[pos + 22:pos + 22 + n].decode('latin-1')
                crc = struct.unpack_from('<H', data, pos + 22 + n)[0]
                at = pos + base - 2                  # the size of the first extended header
            else:
                base = struct.unpack_from('<H', data, pos)[0]
                crc = struct.unpack_from('<H', data, pos + 21)[0]
                at = pos + 24
            # the extended headers: a size (2 bytes), then that many bytes: a
            # type, its data, and the size of the next one
            while True:
                s = struct.unpack_from('<H', data, at)[0]
                if not s:
                    break
                kind, body = data[at + 2], data[at + 3:at + s]
                if kind == 1:
                    name = body.decode('latin-1')
                elif kind == 2:
                    directory = body.decode('latin-1').replace('\xff', '/')
                at += s
                skipped += s
            if level == 1:
                start, packed = pos + base + skipped, packed - skipped
            else:
                start = pos + base
        path = (directory + name).replace('\\', '/')
        yield path, method, data[start:start + packed], size, crc
        pos = start + packed


def extract_one(method, packed, size, crc):
    if method in ('-lhd-',):
        return None
    if method in ('-lh0-', '-lz4-'):
        out = packed[:size]
    elif method in ('-lh5-', '-lh6-', '-lh7-'):
        out = decode_lh(packed, int(method[3]), size)
    else:
        raise ValueError(f'method {method} is not supported')
    if crc16(out) != crc:
        raise ValueError('CRC error')
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('-l', action='store_true', help='list')
    ap.add_argument('archive')
    ap.add_argument('-C', dest='directory', default='.')
    ap.add_argument('patterns', nargs='*')
    args = ap.parse_args()
    data = open(args.archive, 'rb').read()
    count = 0
    for path, method, packed, size, crc in entries(data):
        if args.patterns and not any(p in path for p in args.patterns):
            continue
        if args.l:
            print(f'{size:9d} {method} {path}')
            continue
        out = extract_one(method, packed, size, crc)
        if out is None:
            continue
        to = os.path.join(args.directory, path)
        os.makedirs(os.path.dirname(to) or '.', exist_ok=True)
        with open(to, 'wb') as f:
            f.write(out)
        count += 1
    if not args.l:
        print(f'lha: {count} files extracted', file=sys.stderr)


if __name__ == '__main__':
    main()
