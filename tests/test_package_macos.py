"""Portable DMG staging and POSIX installed-state checks (no Mac required)."""
from pathlib import Path
import os
import plistlib
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest import mock
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import package_macos as package

class MacPackagingTests(unittest.TestCase):
    def test_bundle_has_framework_icon_and_committed_defaults(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);build=root/'build';build.mkdir();(build/'amos').write_bytes(b'fixture executable')
            framework=root/'SDL2.framework';(framework/'Versions/A').mkdir(parents=True)
            (framework/'Versions/A/SDL2').write_bytes(b'fixture framework')
            (framework/'Versions/Current').symlink_to('A')
            (framework/'SDL2').symlink_to('Versions/Current/SDL2')
            stage=root/'stage';stage.mkdir()
            app=package.stage_bundle(build,framework,stage,'0.1.3')
            info=plistlib.loads((app/'Contents/Info.plist').read_bytes())
            self.assertEqual(info['CFBundleExecutable'],'amos')
            self.assertEqual(info['CFBundleIconFile'],'amos.icns')
            self.assertEqual(info['LSMinimumSystemVersion'],'11.0')
            doc=info['CFBundleDocumentTypes'][0]
            self.assertEqual(doc['LSItemContentTypes'],['io.github.dschwen.amosystem.program'])
            self.assertEqual(doc['CFBundleTypeRole'],'Editor')
            self.assertIn('AMOS',doc['CFBundleTypeExtensions'])
            self.assertEqual(doc['CFBundleTypeIconFile'],'amos.icns')
            exported=info['UTExportedTypeDeclarations'][0]
            self.assertEqual(exported['UTTypeIdentifier'],'io.github.dschwen.amosystem.program')
            self.assertIn('AMOS',exported['UTTypeTagSpecification']['public.filename-extension'])
            self.assertEqual(exported['UTTypeIconFile'],'amos.icns')
            self.assertEqual(os.readlink(stage/'Applications'),'/Applications')
            self.assertTrue((app/'Contents/Frameworks/SDL2.framework/SDL2').is_file())
            config='AMOS/APSystem/AMOSPro_Editor_Config'
            self.assertEqual((app/'Contents/Resources'/config).read_bytes(),subprocess.check_output(['git','show','HEAD:'+config],cwd=ROOT))
            self.assertFalse((app/'Contents/Resources/AMOS/AMOSPro').exists())
            self.assertTrue((app/'Contents/Resources/licenses/SDL2-LICENSE.txt').is_file())
            self.assertTrue(package.bundle_manifest(app))
            (framework/'external').symlink_to('/etc/passwd')
            with self.assertRaises(ValueError):package.validate_framework(framework)

    def test_universal_sdl_inspects_only_intel_dependencies(self):
        def output(*args):
            if args[0].endswith('lipo'): return 'x86_64 arm64'
            self.assertIn('-arch',args); self.assertIn('x86_64',args)
            return 'SDL2:\n @rpath/SDL2.framework/Versions/A/SDL2 (compatibility version 1.0.0)\n /usr/lib/libSystem.B.dylib (compatibility version 1.0.0)'
        with mock.patch.object(package,'run',side_effect=output):
            self.assertEqual(package.verify_binary(Path('SDL2'),True),['/usr/lib/libSystem.B.dylib'])
            with self.assertRaises(ValueError):package.verify_binary(Path('amos'))

    def test_numeric_version(self):
        self.assertEqual(package.validate_version('00.1.03'),'0.1.3')
        for value in ('../app','1.2','v1.2.3','1.2.3;false'):
            with self.assertRaises(ValueError):package.validate_version(value)

    @unittest.skipUnless(shutil.which('g++'),'requires native C++ compiler')
    def test_per_user_mirror_refresh_preserves_settings_and_scripts(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp);src=root/'resources';dest=root/'runtime'
            (src/'APSystem').mkdir(parents=True)
            (src/'APSystem/AMOSPro_Editor_Config').write_text('defaults')
            (src/'sample.AMOS').write_text('original script')
            (src/'immutable').write_text('old resource')
            cpp=root/'test.cpp';exe=root/'seed'
            cpp.write_text('#include "host/installed_state.h"\nint main(int n,char **v) { return n==3 && amos::host::refresh_posix_resources(v[1],v[2]) ? 0 : 1; }\n')
            subprocess.run(['g++','-std=c++20','-Isrc',str(cpp),'src/host/installed_state.cpp','-o',str(exe)],cwd=ROOT,check=True)
            subprocess.run([exe,src,dest],check=True)
            (dest/'APSystem/AMOSPro_Editor_Config').write_text('user settings')
            (dest/'sample.AMOS').write_text('edited script')
            (dest/'my-project').write_text('user data')
            (src/'APSystem/AMOSPro_Editor_Config').write_text('new defaults')
            (src/'immutable').write_text('new resource')
            subprocess.run([exe,src,dest],check=True)
            self.assertEqual((dest/'APSystem/AMOSPro_Editor_Config').read_text(),'user settings')
            self.assertEqual((dest/'sample.AMOS').read_text(),'edited script')
            self.assertEqual((dest/'my-project').read_text(),'user data')
            self.assertEqual((dest/'immutable').read_text(),'new resource')
            (src/'bad-link').symlink_to('/etc/passwd')
            self.assertEqual(subprocess.run([exe,src,dest]).returncode,1)
