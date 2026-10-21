#!/usr/bin/env python3
"""Convert an IFF ILBM picture (as written by AMOS Save Iff) to PNG.

  ilbm2png.py IN.iff [OUT.png]
"""

import struct
import sys

from PIL import Image


def chunks(data, pos, end):
    while pos + 8 <= end:
        cid = data[pos:pos + 4]
        size = struct.unpack_from('>L', data, pos + 4)[0]
        yield cid, data[pos + 8:pos + 8 + size]
        pos += 8 + size + (size & 1)


def unpack_byterun1(data, size):
    out = bytearray()
    i = 0
    while len(out) < size and i < len(data):
        n = data[i] - 256 if data[i] > 127 else data[i]
        i += 1
        if n >= 0:
            out += data[i:i + n + 1]
            i += n + 1
        elif n != -128:
            out += bytes([data[i]]) * (1 - n)
            i += 1
    return bytes(out)


def ilbm_to_image(path):
    with open(path, 'rb') as f:
        data = f.read()
    if data[:4] != b'FORM' or data[8:12] != b'ILBM':
        raise ValueError('not an IFF ILBM file')
    bmhd = cmap = body = None
    camg = 0
    for cid, payload in chunks(data, 12, len(data)):
        if cid == b'BMHD':
            bmhd = struct.unpack('>HHhhBBBBHBBhh', payload[:20])
        elif cid == b'CMAP':
            cmap = payload
        elif cid == b'CAMG':
            camg = struct.unpack('>L', payload[:4])[0]
        elif cid == b'BODY':
            body = payload
    width, height, _, _, planes, masking, compression = bmhd[:7]
    row_bytes = ((width + 15) // 16) * 2
    depth = planes + (1 if masking == 1 else 0)
    size = row_bytes * depth * height
    raw = unpack_byterun1(body, size) if compression == 1 else body
    pixels = bytearray(width * height)
    for y in range(height):
        for p in range(planes):
            off = (y * depth + p) * row_bytes
            row = raw[off:off + row_bytes]
            for x in range(width):
                if row[x >> 3] & (0x80 >> (x & 7)):
                    pixels[y * width + x] |= 1 << p
    img = Image.frombytes('P', (width, height), bytes(pixels))
    palette = list(cmap or b'')
    img.putpalette(palette + [0] * (768 - len(palette)))
    if camg & 0x8000 and width >= 640 and height < 300:      # hires: double height
        img = img.resize((width, height * 2), Image.NEAREST)
    return img.convert('RGB')


def main():
    if len(sys.argv) not in (2, 3):
        sys.exit(__doc__)
    out = sys.argv[2] if len(sys.argv) == 3 else sys.argv[1].rsplit('.', 1)[0] + '.png'
    ilbm_to_image(sys.argv[1]).save(out)
    print(out)


if __name__ == '__main__':
    main()
