import os
import random
import shutil
import sys
import tempfile
import unittest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import adf  # noqa: E402


def tree(base):
    random.seed(7)
    os.makedirs(os.path.join(base, 'S'))
    os.makedirs(os.path.join(base, 'Deep/er/still'))
    files = {'Startup.txt': b'hello\n', 'Empty': b'', 'S/startup-sequence': b'echo hi\n',
             'Big.bin': bytes(random.randrange(256) for _ in range(100000)),
             'Exact.bin': bytes(random.randrange(256) for _ in range(488 * 3)),
             'Deep/er/still/f%3F': b'x' * 513, 'Zeta': b'z', 'alpha': b'a'}
    for name, data in files.items():
        with open(os.path.join(base, name), 'wb') as f:
            f.write(data)
    return files


class AdfTests(unittest.TestCase):
    def roundtrip(self, ffs, intl):
        with tempfile.TemporaryDirectory() as t:
            src = os.path.join(t, 'src'); os.makedirs(src)
            files = tree(src)
            image = adf.pack(src, 'Test Disk', ffs, intl)
            self.assertEqual(len(image), 901120)
            disk = adf.Adf(image)
            self.assertEqual((disk.label, disk.ffs), (b'Test Disk', ffs))
            for name, data in files.items():
                path = name.replace('%3F', '?')
                e = disk.lookup(path.upper())       # AmigaDOS ignores the case
                self.assertIsNotNone(e, path)
                self.assertEqual(disk.read(e), data, path)
            self.assertIsNone(disk.lookup('nothere'))
            # every block of the file system checks (header checksums), bitmap agrees
            used = 1760 - 2 - disk.free_blocks()
            self.assertGreater(used, 200)
            out = os.path.join(t, 'out')
            self.assertEqual(adf.command_line(['extract', os.path.join(t, 'd.adf'), out]), 1)
            with open(os.path.join(t, 'd.adf'), 'wb') as f:
                f.write(image)
            self.assertEqual(adf.command_line(['extract', os.path.join(t, 'd.adf'), out]), 0)
            for name, data in files.items():
                with open(os.path.join(out, name), 'rb') as f:
                    self.assertEqual(f.read(), data, name)

    def test_ffs(self):
        self.roundtrip(True, False)

    def test_ofs(self):
        self.roundtrip(False, False)

    def test_intl(self):
        self.roundtrip(True, True)

    def test_hash_and_names(self):
        self.assertEqual(adf.hash_name(b'Startup-Sequence', False), adf.hash_name(b'STARTUP-SEQUENCE', False))
        self.assertTrue(adf.same_name(b'\xe9a', b'\xc9A', True))
        self.assertFalse(adf.same_name(b'\xe9a', b'\xc9A', False))
        self.assertEqual(adf.unescape(adf.escape(b'a:b?c.')), b'a:b?c.')

    def test_damaged(self):
        with tempfile.TemporaryDirectory() as t:
            src = os.path.join(t, 's'); os.makedirs(src); tree(src)
            image = bytearray(adf.pack(src))
            image[880 * 512 + 100] ^= 1
            with self.assertRaises(adf.AdfError):
                adf.Adf(bytes(image))
            with self.assertRaises(adf.AdfError):
                adf.Adf(b'x' * 10)


if __name__ == '__main__':
    unittest.main()
