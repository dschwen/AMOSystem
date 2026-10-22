"""Keyboard and mouse events for an X display, through the XTest extension
(ctypes, no extra packages): the harness drives FS-UAE with them."""

import ctypes
import ctypes.util
import time

_x11 = ctypes.CDLL(ctypes.util.find_library('X11'))
_xtst = ctypes.CDLL(ctypes.util.find_library('Xtst'))
_x11.XOpenDisplay.restype = ctypes.c_void_p
_x11.XOpenDisplay.argtypes = [ctypes.c_char_p]
_x11.XStringToKeysym.restype = ctypes.c_ulong
_x11.XStringToKeysym.argtypes = [ctypes.c_char_p]
_x11.XKeysymToKeycode.restype = ctypes.c_ubyte
_x11.XKeysymToKeycode.argtypes = [ctypes.c_void_p, ctypes.c_ulong]
_x11.XFlush.argtypes = [ctypes.c_void_p]
_x11.XCloseDisplay.argtypes = [ctypes.c_void_p]
_xtst.XTestFakeKeyEvent.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_int, ctypes.c_ulong]
_xtst.XTestFakeButtonEvent.argtypes = [ctypes.c_void_p, ctypes.c_uint, ctypes.c_int, ctypes.c_ulong]
_xtst.XTestFakeMotionEvent.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_ulong]
_xtst.XTestFakeRelativeMotionEvent.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_ulong]


class Display:
    def __init__(self, name):
        self.d = _x11.XOpenDisplay(name.encode())
        if not self.d:
            raise RuntimeError(f'cannot open display {name}')

    def close(self):
        _x11.XCloseDisplay(self.d)

    def key(self, keysym, hold=0.05):
        """Press and release a key given by its X keysym name ("a", "Return"),
        with "shift+", "ctrl+" or "alt+" in front for modifiers."""
        *mods, name = keysym.split('+') if len(keysym) > 1 else [keysym]
        modifiers = {'shift': 'Shift_L', 'ctrl': 'Control_L', 'alt': 'Alt_L'}
        codes = []
        for k in [modifiers[m] for m in mods] + [name]:
            code = _x11.XKeysymToKeycode(self.d, _x11.XStringToKeysym(k.encode()))
            if not code:
                raise ValueError(f'unknown key {k}')
            codes.append(code)
        for code in codes:
            _xtst.XTestFakeKeyEvent(self.d, code, 1, 0)
            _x11.XFlush(self.d)
            time.sleep(hold)
        for code in reversed(codes):
            _xtst.XTestFakeKeyEvent(self.d, code, 0, 0)
            _x11.XFlush(self.d)
            time.sleep(hold)

    def keys(self, keysyms, gap=0.05):
        for k in keysyms:
            self.key(k)
            time.sleep(gap)

    def move(self, x, y):
        _xtst.XTestFakeMotionEvent(self.d, -1, x, y, 0)
        _x11.XFlush(self.d)

    def move_by(self, dx, dy):
        _xtst.XTestFakeRelativeMotionEvent(self.d, dx, dy, 0)
        _x11.XFlush(self.d)

    def button(self, n, down):
        _xtst.XTestFakeButtonEvent(self.d, n, 1 if down else 0, 0)
        _x11.XFlush(self.d)
