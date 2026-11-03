"""Validate real icon images and packaging integration in isolated directories."""
from pathlib import Path
import plistlib
import sys
import tempfile
import unittest
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import install_icons
import windows_resources

class IconTests(unittest.TestCase):
    def test_formats_and_source(self):
        logo=Image.open(ROOT/'data/icons/AMOS_LOGO.png').convert('RGBA');self.addCleanup(logo.close)
        ico=Image.open(ROOT/'data/icons/amos.ico');self.addCleanup(ico.close)
        self.assertEqual(ico.ico.sizes(),{(n,n) for n in (16,24,32,48,64,128,256)})
        self.assertEqual(ico.ico.getimage((256,256)).convert('RGBA').tobytes(),logo.tobytes())
        icns=Image.open(ROOT/'data/icons/amos.icns');self.addCleanup(icns.close);icns.load()
        self.assertEqual(icns.size,(1024,1024))
        self.assertEqual(windows_resources.icon(),(ROOT/'data/icons/amos.ico').read_bytes())
        png=Image.open(ROOT/'data/icons/hicolor/256x256/apps/io.github.dschwen.amosystem.png');self.addCleanup(png.close)
        self.assertEqual(png.tobytes(),logo.tobytes())
        self.assertEqual(logo.getpixel((0,0))[3],0)

    def test_linux_install_uninstall_preserves_others(self):
        with tempfile.TemporaryDirectory() as temp:
            prefix=Path(temp)
            other=prefix/'share/icons/hicolor/other.png';other.parent.mkdir(parents=True);other.write_bytes(b'other')
            install_icons.linux(prefix)
            self.assertTrue((prefix/'share/applications/io.github.dschwen.amosystem.desktop').exists())
            self.assertEqual(len(list((prefix/'share/icons/hicolor').glob('*/apps/*.png'))),8)
            self.assertEqual(len(list((prefix/'share/icons/hicolor').glob('*/mimetypes/*.png'))),8)
            install_icons.linux(prefix,uninstall=True)
            self.assertEqual(other.read_bytes(),b'other')
            self.assertFalse((prefix/'share/applications/io.github.dschwen.amosystem.desktop').exists())
            self.assertFalse(list((prefix/'share/icons/hicolor').glob('*/apps/*.png')))

    def test_macos_preserves_bundle_and_sets_document_icons(self):
        with tempfile.TemporaryDirectory() as temp:
            app=Path(temp)/'AMOS Professional.app';(app/'Contents').mkdir(parents=True)
            info={'CFBundleIdentifier':'test.bundle', 'CFBundleDocumentTypes':[
                {'LSItemContentTypes':['io.github.dschwen.amosystem.program']},
                {'CFBundleTypeExtensions':['txt'],'CFBundleTypeIconFile':'text.icns'}],
                'UTExportedTypeDeclarations':[{'UTTypeTagSpecification':{'public.filename-extension':'amos'}}]}
            plist=app/'Contents/Info.plist';plist.write_bytes(plistlib.dumps(info))
            install_icons.macos(app)
            result=plistlib.loads(plist.read_bytes())
            self.assertEqual(result['CFBundleIdentifier'],'test.bundle')
            self.assertEqual(result['CFBundleIconFile'],'amos.icns')
            self.assertEqual(result['CFBundleDocumentTypes'][0]['CFBundleTypeIconFile'],'amos.icns')
            self.assertEqual(result['CFBundleDocumentTypes'][1]['CFBundleTypeIconFile'],'text.icns')
            self.assertEqual(result['UTExportedTypeDeclarations'][0]['UTTypeIconFile'],'amos.icns')
            self.assertEqual((app/'Contents/Resources/amos.icns').read_bytes(),(ROOT/'data/icons/amos.icns').read_bytes())
