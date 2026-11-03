#!/usr/bin/env python3
"""Convert the original AMOS logo to platform icon containers (requires Pillow)."""
import argparse
import io
from pathlib import Path
import struct
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SIZES = (16, 24, 32, 48, 64, 128, 256, 512)


def generate(source, out):
    image = Image.open(source).convert('RGBA')
    edge = max(image.size)
    canvas = Image.new('RGBA', (edge, edge))
    canvas.paste(image, ((edge-image.width)//2, (edge-image.height)//2))
    def scaled(size):
        return canvas.resize((size, size), Image.Resampling.LANCZOS)
    out.mkdir(parents=True, exist_ok=True)
    # Pillow's ICO writer emits PNG-compressed entries and includes all sizes.
    canvas.save(out/'amos.ico', format='ICO', sizes=[(n,n) for n in SIZES if n <= 256])
    chunks = []
    for kind, size in ((b'icp4',16),(b'icp5',32),(b'icp6',64),(b'ic07',128),
                       (b'ic08',256),(b'ic09',512),(b'ic10',1024)):
        png = io.BytesIO(); scaled(size).save(png, format='PNG')
        data = png.getvalue()
        chunks.append(kind + struct.pack('>I',len(data)+8) + data)
    body = b''.join(chunks)
    (out/'amos.icns').write_bytes(b'icns' + struct.pack('>I',len(body)+8) + body)
    for size in SIZES:
        directory = out/'hicolor'/f'{size}x{size}'/'apps'
        directory.mkdir(parents=True, exist_ok=True)
        scaled(size).save(directory/'io.github.dschwen.amosystem.png')


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source', type=Path, default=ROOT/'data/icons/AMOS_LOGO.png')
    ap.add_argument('--out', type=Path, default=ROOT/'data/icons')
    args=ap.parse_args(); generate(args.source,args.out)

if __name__=='__main__': main()
