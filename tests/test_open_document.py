"""Shared host document activation: load into an editor, never execute on open."""
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import unittest

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import amostok


@unittest.skipUnless((ROOT/'build/host/amos').is_file(),'build Linux host first')
class OpenDocumentTests(unittest.TestCase):
    def test_native_paths_and_load_without_execution(self):
        with tempfile.TemporaryDirectory(prefix='amos-document-') as temp:
            folder=Path(temp)
            resources=folder/'resources';shutil.copytree(ROOT/'AMOS',resources)
            document=folder/'Document café with spaces.AMOS'
            tables=amostok.Tables([str(resources/'APSystem')])
            amostok.tokenize_program('Open Out 1,"START:document-executed.txt" : Close 1\nSystem\n',tables).save(str(document))
            (folder/'empty.conf').write_text('')
            env=dict(os.environ,AMOS_HOME=str(resources),AMOS_DISPLAY='none',AMOS_CONFIG=str(folder/'empty.conf'),AMOS_DEVICES=str(folder/'devices'))
            for name in (str(document),document.name):
                with self.subTest(path=name):
                    process=subprocess.Popen([str(ROOT/'build/host/amos'),'--open-document',name],cwd=folder,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
                    try:
                        time.sleep(2)
                        if process.poll() is not None:
                            self.fail('Editor exited: '+process.communicate()[0].decode('latin-1'))
                        self.assertFalse((folder/'document-executed.txt').exists())
                    finally:
                        if process.poll() is None:process.terminate()
                        process.communicate(timeout=5)
            for args in (['--open-document'],['--open-document','missing.AMOS'],['--open-document',document.name,'-r']):
                result=subprocess.run([str(ROOT/'build/host/amos'),*args],cwd=folder,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=5)
                self.assertNotEqual(result.returncode,0,result.stdout)

if __name__=='__main__':unittest.main()
