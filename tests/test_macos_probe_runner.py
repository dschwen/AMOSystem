"""Exercise Mac runner quarantine/sign/execute ordering with isolated fake tools."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]

class RunnerTests(unittest.TestCase):
    def test_quarantine_is_reported_without_modification_or_execution(self):
        for quarantined in (True,False):
            with self.subTest(quarantined=quarantined), tempfile.TemporaryDirectory() as temp:
                root=Path(temp);bin_dir=root/'bin';bin_dir.mkdir()
                shutil.copy2(ROOT/'tests/macos/run-probes.sh',root/'run-probes.sh')
                scripts={
                    'xattr': 'if [ "$1" != -p ]; then echo modified >> events; exit 1; fi\nexit '+('0' if quarantined else '1'),
                    'codesign': 'echo signed >> events\nexit 0',
                    'sw_vers': 'echo test-macos',
                }
                for name,body in scripts.items():
                    p=bin_dir/name;p.write_text('#!/bin/sh\n'+body+'\n');p.chmod(0o755)
                probe=root/'address-probe';probe.write_text('#!/bin/sh\necho executed >> events\nprintf \'{"probe":true}\\n\'\n');probe.chmod(0o755)
                env=dict(os.environ,PATH=str(bin_dir)+os.pathsep+os.environ['PATH'])
                result=subprocess.run(['sh',str(root/'run-probes.sh')],env=env,capture_output=True,text=True)
                if quarantined:
                    self.assertEqual(result.returncode,1)
                    self.assertIn('download quarantine',result.stdout)
                    self.assertFalse((root/'events').exists())
                    self.assertFalse((root/'address-probe-runtime.json').exists())
                else:
                    self.assertEqual(result.returncode,0,result.stderr)
                    self.assertEqual((root/'events').read_text().splitlines(),['signed','executed'])
                    self.assertIn('"probe":true',(root/'address-probe-runtime.json').read_text())
