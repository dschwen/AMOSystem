#!/usr/bin/env python3
"""Generate deterministic native icon and version resources (no image dependencies)."""
import argparse
from pathlib import Path
import re
import struct


def icon():
    glyph = ('0011100', '0110110', '1100011', '1100011', '1111111', '1100011', '1100011')
    images = []
    for size in (16, 32, 48, 256):
        pixels = bytearray()
        scale = max(1, size // 10)
        origin = (size - 7 * scale) // 2
        for y in reversed(range(size)):
            for x in range(size):
                gx, gy = (x-origin)//scale, (y-origin)//scale
                lit = 0 <= gx < 7 and 0 <= gy < 7 and glyph[gy][gx] == '1'
                r,g,b = (255,204,51) if lit else (11,27,77)
                pixels.extend((b,g,r,255))
        mask = bytes(((size + 31)//32)*4*size)
        header = struct.pack('<IiiHHIIiiII',40,size,size*2,1,32,0,len(pixels)+len(mask),0,0,0,0)
        images.append((size,header+pixels+mask))
    offset = 6 + 16*len(images)
    result = bytearray(struct.pack('<HHH',0,1,len(images)))
    for size,data in images:
        result.extend(struct.pack('<BBBBHHII',size%256,size%256,0,0,1,32,len(data),offset))
        offset += len(data)
    return bytes(result)+b''.join(data for _,data in images)


def write_if_changed(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.exists() or path.read_bytes() != data:
        path.write_bytes(data)


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--version',required=True)
    args=ap.parse_args()
    if not re.fullmatch(r'\d+\.\d+\.\d+',args.version): ap.error('version must be MAJOR.MINOR.PATCH')
    nums=[int(n) for n in args.version.split('.')]
    if nums[0]>255 or nums[1]>255 or nums[2]>65535: ap.error('version exceeds MSI numeric limits')
    args.version='.'.join(map(str,nums))
    ico=args.out.parent/'amos.ico'
    write_if_changed(ico,icon())
    version=','.join(str(n) for n in nums)+',0'
    rc=f'''#include <windows.h>
0 ICON "{ico.resolve().as_posix()}"
1 VERSIONINFO
FILEVERSION {version}
PRODUCTVERSION {version}
FILETYPE VFT_APP
BEGIN
 BLOCK "StringFileInfo"
 BEGIN
  BLOCK "040904b0"
  BEGIN
   VALUE "FileDescription", "AMOS Professional\\0"
   VALUE "FileVersion", "{args.version}\\0"
   VALUE "ProductName", "AMOS Professional\\0"
   VALUE "ProductVersion", "{args.version}\\0"
   VALUE "CompanyName", "AMOSystem contributors\\0"
  END
 END
 BLOCK "VarFileInfo"
 BEGIN
  VALUE "Translation", 0x0409, 1200
 END
END
'''
    write_if_changed(args.out,rc.encode())


if __name__=='__main__': main()
