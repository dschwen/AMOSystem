"""Pure conversion tests and real X11/SDL clipboard editor integration.

Run after make host: python3 -m unittest discover -s tests -p test_clipboard.py
Uses an isolated Xvfb clipboard, never the user's desktop clipboard.
"""
import ctypes
import ctypes.util
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
import threading
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(ROOT / 'tests'))

class TextTests(unittest.TestCase):
    def test_conversion_and_limits(self):
        code = r'''
#include "host/clipboard_text.h"
#include "editor/clipboard.h"
#include <assert.h>
using namespace amos::host::clipboard;
int main() {
    // Default menus include Help at root 8; Clipboard must not alias it.
    uint8_t menus[9 * 8] = {};
    for (unsigned i = 0; i < 8; ++i) menus[i*8] = i + 1;
    assert(amos::ed::clipboard_menu_slot(menus) == 9);
    menus[7*8] = 10;
    assert(amos::ed::clipboard_menu_slot(menus) == 8);
    std::string s, u;
    assert(decode("caf\xc3\xa9\r\n\t\r", s));
    assert(s == "caf\xe9\n \n");
    assert(encode(s, u) && u == "caf\xc3\xa9\n \n");
    for (auto p : {"\xc0\x80", "\xed\xa0\x80", "\xf0\x9f\x98\x80", "\xc3", "\x1b", "\xc2\x80"})
        assert(!decode(p, s));
    std::string large(Limit + 1, 'a'); assert(!decode(large.c_str(), s));
    large.assign(Limit, 'a'); assert(decode(large.c_str(), s));
    s = "Print 1\n"; assert(command_line(s) && s == "Print 1");
    s = "Print 1\nPrint 2\n"; assert(!command_line(s));
    s = "\n\n"; assert(!command_line(s));
}
'''
        with tempfile.TemporaryDirectory() as t:
            source = Path(t) / 'test.cpp'; source.write_text(code)
            binary = Path(t) / 'test'
            subprocess.run(['g++', '-std=c++20', '-Isrc', str(source), '-o', str(binary)], cwd=ROOT, check=True)
            subprocess.run([str(binary)], check=True)

@unittest.skipUnless(shutil.which('Xvfb') and (ROOT / 'build/host/amos').exists(), 'requires Xvfb and Linux host')
class EditorClipboardTests(unittest.TestCase):
    def test_source_and_direct_mode(self):
        import amostok
        import run
        from xinput import Display
        xvfb, display_name = run.start_xvfb()
        old_display = os.environ.get('DISPLAY')
        os.environ['DISPLAY'] = display_name
        sdl = ctypes.CDLL(ctypes.util.find_library('SDL2-2.0'))
        sdl.SDL_Init.argtypes = [ctypes.c_uint32]
        sdl.SDL_CreateWindow.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_uint32]
        sdl.SDL_CreateWindow.restype = ctypes.c_void_p
        sdl.SDL_SetClipboardText.argtypes = [ctypes.c_char_p]
        sdl.SDL_GetClipboardText.restype = ctypes.c_void_p
        sdl.SDL_free.argtypes = [ctypes.c_void_p]
        sdl.SDL_PollEvent.argtypes = [ctypes.c_void_p]
        process = None
        keyboard = None
        try:
            self.assertEqual(sdl.SDL_Init(0x20), 0)
            self.assertTrue(sdl.SDL_CreateWindow(b'Clipboard test owner', 0, 0, 10, 10, 8))
            events = ctypes.create_string_buffer(64)
            def pump(seconds=.4):
                end = time.monotonic() + seconds
                while time.monotonic() < end:
                    while sdl.SDL_PollEvent(events): pass
                    time.sleep(.005)
            def read():
                p = sdl.SDL_GetClipboardText()
                try: return ctypes.string_at(p).decode('utf8')
                finally: sdl.SDL_free(p)
            def write(text):
                self.assertEqual(sdl.SDL_SetClipboardText(text.encode('utf8')), 0)
                pump(.05)
            keyboard = Display(display_name)
            def key(name):
                keyboard.key(name, hold=.03); pump()
                self.assertIsNone(process.poll(), 'AMOS unexpectedly exited')
            with tempfile.TemporaryDirectory(prefix='amos-clipboard-') as t:
                folder = Path(t); resources = folder / 'resources'
                shutil.copytree(ROOT / 'AMOS', resources)
                doc = folder / 'source.AMOS'
                tables = amostok.Tables([str(resources / 'APSystem')])
                amostok.tokenize_program('Rem original\nRem second\n', tables).save(str(doc))
                (folder / 'empty.conf').write_text('')
                env = dict(os.environ, SDL_AUDIODRIVER='dummy', AMOS_HOME=str(resources), AMOS_SCALE='1',
                           AMOS_CONFIG=str(folder / 'empty.conf'), AMOS_DEVICES=str(folder / 'devices'))
                env.pop('AMOS_DISPLAY', None)
                with (ROOT / 'build/clipboard-test.log').open('w+') as log:
                    process = subprocess.Popen([str(Path(os.environ.get('AMOS_CLIPBOARD_TEST_BINARY', ROOT / 'build/host/amos'))), '--open-document', str(doc)],
                                               cwd=folder, env=env, stdout=log, stderr=log)
                    pump(3)
                    key('ctrl+shift+Up')  # Dismiss startup information.
                    def menu(command):
                        # Default configuration: Clipboard is after Help (root 9).
                        # Exercise real hit-testing/selection, not command IDs.
                        keyboard.move_by(-3000,-3000); pump()
                        keyboard.button(3,True); pump()
                        keyboard.move_by(480,0); pump()
                        keyboard.move_by(0,24 + (command-1)*16); pump()
                        if os.environ.get('AMOS_CLIPBOARD_MENU_SHOT'):
                            subprocess.run(['scrot','-o',str(ROOT/'build/clipboard-menu.png')],check=True)
                        keyboard.button(3,False); pump()
                    key('ctrl+shift+Up')
                    key('ctrl+b')
                    for _ in range(3): key('Right')
                    menu(1)
                    self.assertEqual(read().lower(), 'rem')
                    # Replace selection; one undo restores the original selection/text.
                    write('Print 1\nRem')
                    menu(3)
                    def source_contents():
                        # REM detokenization can add a trailing display space.
                        return read().replace(' \n', '\n')
                    def source_text():
                        key('ctrl+shift+Up'); key('ctrl+b'); key('ctrl+shift+Down'); key('ctrl+c')
                        return source_contents()
                    self.assertEqual(source_text().lower(), 'print 1\nrem original\nrem second\n')
                    key('ctrl+u')
                    key('ctrl+c')
                    self.assertEqual(read().lower(), 'rem')
                    key('ctrl+shift+u')
                    self.assertEqual(source_text().lower(), 'print 1\nrem original\nrem second\n')
                    # Selection replacement rejection must leave the source intact.
                    write('x' * 251); key('ctrl+v')
                    key('ctrl+c')
                    self.assertEqual(source_contents().lower(), 'print 1\nrem original\nrem second\n')
                    write('emoji \U0001f600'); key('ctrl+v'); key('ctrl+c')
                    self.assertEqual(source_contents().lower(), 'print 1\nrem original\nrem second\n')
                    menu(2)
                    write('Rem cut replacement\n\nRem tail\n'); key('ctrl+v')
                    self.assertEqual(source_text().lower(), 'rem cut replacement\n\nrem tail\n')
                    key('ctrl+u'); key('ctrl+u'); key('ctrl+c')
                    self.assertEqual(source_contents().lower(), 'print 1\nrem original\nrem second\n')
                    # Direct-mode paste never runs, supports Unicode and cursor insertion.
                    key('Escape'); pump(1)
                    command = 'Open Out 1,"START:executed.txt" : Close 1'
                    write(command + '\r\n'); key('ctrl+v')
                    self.assertFalse((folder / 'executed.txt').exists())
                    key('ctrl+c'); self.assertEqual(read(), command)
                    write('Print 1\nPrint 2'); key('ctrl+v')
                    key('ctrl+c'); self.assertEqual(read(), command)
                    key('ctrl+x'); key('ctrl+c')  # Empty copy leaves OS contents alone.
                    self.assertEqual(read(), command)
                    write('Print "café"'); key('ctrl+v')
                    key('Home'); key('shift+Right'); key('shift+Right'); key('shift+Right')
                    key('ctrl+c'); self.assertEqual(read(), 'Pri')
                    write('REM'); key('ctrl+v'); key('ctrl+c')
                    self.assertEqual(read(), 'REMnt "café"')
                    # Capacity rejection is atomic.
                    write('x' * 161); key('ctrl+v'); key('ctrl+c')
                    self.assertEqual(read(), 'REMnt "café"')
                    key('ctrl+x')
                    write('Print 1')
                    held = threading.Thread(target=keyboard.key, args=('ctrl+v',), kwargs={'hold': .8})
                    held.start()
                    while held.is_alive(): pump(.05)
                    held.join(); pump()
                    key('ctrl+c'); self.assertEqual(read(), 'Print 1')
                    key('ctrl+x')
                    write('Input A$'); key('ctrl+v'); key('Return'); pump(.5)
                    write('external clipboard sentinel'); key('ctrl+c'); pump(.5)
                    self.assertEqual(read(), 'external clipboard sentinel')
                    key('ctrl+x')
                    write(command); key('ctrl+v'); key('Return'); pump(1)
                    self.assertTrue((folder / 'executed.txt').exists())
        finally:
            if process and process.poll() is None:
                if sys.exc_info()[0] and shutil.which("scrot"):
                    subprocess.run(["scrot", "-o", str(ROOT / "build/clipboard-test.png")], env=dict(os.environ), check=False)
                process.terminate(); process.wait(timeout=5)
            if keyboard: keyboard.close()
            sdl.SDL_Quit()
            xvfb.terminate(); xvfb.wait(timeout=5)
            if old_display is None: os.environ.pop('DISPLAY', None)
            else: os.environ['DISPLAY'] = old_display
