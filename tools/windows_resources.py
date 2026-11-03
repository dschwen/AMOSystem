#!/usr/bin/env python3
"""Embed the AMOS logo icon and generate Windows version resources."""
import argparse
from pathlib import Path
import re


def icon():
    return (Path(__file__).resolve().parents[1]/'data/icons/amos.ico').read_bytes()


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
