#!/usr/bin/env python3
"""Extract Debian/Ubuntu MSI build tools locally without sudo; no system install."""
import argparse
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out',type=Path,default=ROOT/'build/msi-tools')
    args=ap.parse_args()
    out=args.out.resolve(); downloads=out/'downloads'; dest=out/'root'
    downloads.mkdir(parents=True,exist_ok=True);dest.mkdir(exist_ok=True)
    candidates=['libmsi-1.0-0','libmsi0']
    libmsi=next((p for p in candidates if subprocess.run(['apt-cache','show',p],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode==0),None)
    if not libmsi: ap.error('no libmsi package available in apt repositories')
    packages=['wixl','wixl-data','msitools',libmsi,'libgcab-1.0-0','libgsf-1-114','libgsf-1-common']
    subprocess.run(['apt-get','download',*packages],cwd=downloads,check=True)
    versions=[]
    for package in sorted(downloads.glob('*.deb')):
        subprocess.run(['dpkg-deb','-x',str(package),str(dest)],check=True)
        versions.append(subprocess.check_output(['dpkg-deb','--show',str(package)],text=True).strip())
    (out/'versions.txt').write_text('\n'.join(versions)+'\n')
    print(f'Local MSI tools: {dest}; dependencies/version record: {out / "versions.txt"}')

if __name__=='__main__':main()
