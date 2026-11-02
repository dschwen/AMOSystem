"""Installer ownership/upgrade regression tests. Run with python3 -m unittest discover -s tests -p test_package_msi.py."""
from pathlib import Path
import sys
import tempfile
import unittest
import xml.etree.ElementTree as ET

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import package_msi as msi


class AuthoringTests(unittest.TestCase):
    def test_version_limits_and_rejection_of_paths(self):
        for invalid in ('1.2','1.2.3.4','../1.2.3','1.0.0-rc1','256.0.0','0.256.0','0.0.65536'):
            with self.subTest(invalid=invalid), self.assertRaises(ValueError):msi.version(invalid)
        self.assertEqual(msi.version('255.255.65535'),'255.255.65535')

    def test_upgrade_retains_component_ownership_and_changes_product_identity(self):
        with tempfile.TemporaryDirectory() as temp:
            base=Path(temp);stage=base/'stage';(stage/'AMOS/APSystem').mkdir(parents=True)
            (stage/'amos.exe').write_text('first payload')
            (stage/'AMOS/APSystem/one').write_text('resource')
            codes=[];guids=[]
            for version in ('0.1.0','0.1.1'):
                source=base/(version+'.wxs');codes.append(msi.author(stage,source,version))
                root=ET.parse(source).getroot()
                guids.append({c.get('Id'):c.get('Guid') for c in root.iter(msi.tag('Component'))})
            self.assertNotEqual(codes[0],codes[1])
            self.assertEqual(guids[0],guids[1])
            self.assertEqual(len(guids[0]),len(set(guids[0].values())))

    def test_association_is_owned_and_quotes_document_as_one_argument(self):
        with tempfile.TemporaryDirectory() as temp:
            base=Path(temp);stage=base/'stage';stage.mkdir();(stage/'amos.exe').write_text('payload')
            source=base/'product.wxs';msi.author(stage,source,'0.1.0')
            root=ET.parse(source).getroot()
            entries=list(root.iter(msi.tag('RegistryValue')))
            command=[r.get('Value') for r in entries if r.get('Key','').endswith('AMOSystem.Program\\shell\\open\\command')]
            self.assertEqual(command,['"[INSTALLDIR]amos.exe" --open-document "%1"'])
            self.assertTrue(all(r.get('Root')=='HKLM' for r in entries))
            self.assertFalse(any('UserChoice' in r.get('Key','') for r in entries))
            self.assertFalse(any(r.get('Name') in ('*','-') for r in entries))
            self.assertFalse(list(root.iter(msi.tag('CustomAction'))))
            self.assertTrue(all(c.get('Win64')=='yes' for c in root.iter(msi.tag('Component'))))

    def test_case_collisions_rejected_before_installer_build(self):
        with tempfile.TemporaryDirectory() as temp:
            base=Path(temp);stage=base/'stage';stage.mkdir()
            (stage/'one.txt').write_text('first');(stage/'ONE.TXT').write_text('second')
            with self.assertRaisesRegex(ValueError,'case-colliding'):
                msi.author(stage,base/'product.wxs','0.1.0',True)


class MsiInspectionTests(unittest.TestCase):
    def test_real_probe_tables_and_extracted_payload(self):
        tools=msi.Tools()
        if not (tools.local/'usr/bin/wixl').exists():
            import shutil
            if not shutil.which('wixl'):self.skipTest('wixl not installed')
        with tempfile.TemporaryDirectory() as temp:
            base=Path(temp);stage=base/'stage';stage.mkdir()
            (stage/'one.txt').write_text('one');(stage/'two.txt').write_text('two')
            import subprocess
            subprocess.run(['x86_64-w64-mingw32-gcc','-shared','-nostdlib','-Wl,--entry,0','-o',str(base/'probe-failure.dll'),str(msi.ROOT/'packaging/windows/probe_failure.c')],check=True)
            source=base/'probe.wxs';installer=base/'probe.msi'
            msi.author(stage,source,'0.1.0',True)
            tools.run('wixl','-a','x64','-o',installer,source)
            manifest={p.name:msi.sha(p) for p in stage.iterdir()}
            tables=msi.inspect(tools,installer,manifest,'0.1.0',True)
            sequence={r['Action']:int(r['Sequence']) for r in tables['InstallExecuteSequence']}
            self.assertLess(sequence['InstallInitialize'],sequence['RemoveExistingProducts'])
            self.assertLess(sequence['RemoveExistingProducts'],sequence['InstallFiles'])
            self.assertLess(sequence['InstallFiles'],sequence['IntentionalProbeFailure'])
            self.assertLess(sequence['IntentionalProbeFailure'],sequence['InstallFinalize'])
            manifest['one.txt']='0'*64
            with self.assertRaisesRegex(ValueError,'payload differs'):
                msi.inspect(tools,installer,manifest,'0.1.0',True)

if __name__=='__main__':unittest.main()
