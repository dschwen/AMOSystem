"""The third-party extensions' tables (tools/ext_gen.py, tools/ext_config.py). Run with
python3 -m unittest discover -s tests -p test_ext_gen.py."""
import json
from pathlib import Path
import struct
import subprocess
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import ext_config
import ext_gen

CONFIG = ROOT / 'AMOS/AMOSPro_Interpreter_Config'


def messages(data):
    values = struct.unpack_from('>I', data, 4)[0]
    at = 8 + values
    size = struct.unpack_from('>I', data, at + 4)[0]
    texts = data[at + 8:at + 8 + size]
    out, i = [], 1
    while texts[i] != 0xFF:
        out.append(texts[i + 1:i + 1 + texts[i]])
        i += 2 + texts[i]
    return out


class ConfigTests(unittest.TestCase):
    def test_extensions_in_their_slots_and_nothing_else_changed(self):
        original = CONFIG.read_bytes()
        patched = ext_config.assign(original, ext_config.parse(None))
        before, after = messages(original), messages(patched)
        self.assertEqual(after[23 - 1], b'AMOSPro_AMCAF.Lib')          # slot 8 is message 23
        self.assertEqual(after[27 - 1], b'AMOSPro_TURBO_Plus.Lib')      # slot 12
        changed = [i + 1 for i, (a, b) in enumerate(zip(before, after)) if a != b]
        self.assertEqual(changed, [23, 27])
        self.assertEqual(len(before), len(after))

    def test_a_slot_can_be_freed_and_the_range_is_checked(self):
        patched = ext_config.assign(CONFIG.read_bytes(), {1: ''})
        self.assertEqual(messages(patched)[16 - 1], b'')
        with self.assertRaises(ValueError):
            ext_config.assign(CONFIG.read_bytes(), {27: 'x'})


class TableTests(unittest.TestCase):
    def test_what_is_committed_is_what_the_generator_writes(self):
        result = subprocess.run([sys.executable, str(ROOT / 'tools/ext_gen.py'), '--check'],
                                capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_manifests(self):
        for key, commands in (('amcaf', 347), ('turbo', 153)):
            m = json.loads(ext_gen.manifest_path(key).read_text())
            self.assertEqual(len(m['commands']), commands)
            for c in m['commands']:
                if c['status'] != 'stub':
                    self.assertTrue((ROOT / c['source']).exists(), c['symbol'])
            by_symbol = {}
            for c in m['commands']:
                by_symbol.setdefault(c['symbol'], set()).add(c['routine'])
            self.assertTrue(all(len(r) == 1 for r in by_symbol.values()), 'a symbol for two routines')

    def test_the_images_keep_the_original_token_offsets(self):
        for key, info in ext_gen.EXTENSIONS.items():
            lib = ROOT / 'AMOS/APSystem' / info['file']
            if not lib.exists():
                self.skipTest(f'{info["file"]} is not in AMOS/APSystem')
            library = ext_gen.read_library(lib)
            image = ext_gen.image_bytes(library)
            table = image[18 + 4 + library['routines'] * 2:][:library['table_size']]
            offsets, p = [], 6
            while struct.unpack_from('>H', table, p)[0]:
                offsets.append(p)
                p += 4
                while table[p] < 128:
                    p += 1
                p += 1
                while table[p] < 128:
                    p += 1
                p += 1
                p += p & 1
            self.assertEqual(offsets, [t.offset for t in library['tokens']])
            sys.path.insert(0, str(ROOT / 'tools'))
            import amostok
            tokens = amostok.TokenTable(table, 6)
            self.assertEqual([e.offset for e in tokens.entries], offsets)


if __name__ == '__main__':
    unittest.main()
