#!/usr/bin/env python3
"""Amiga floppy images (ADF, 880 KiB double density): list, extract, pack.

    adf.py info DISK.adf
    adf.py list [-l] DISK.adf [PATH]
    adf.py extract DISK.adf FOLDER [PATH]
    adf.py pack FOLDER DISK.adf [--label NAME] [--ofs] [--intl]

OFS and FFS, the international variants (the case folding of the hash and of
the name comparison). It is the reference of the host's own driver
(docs/DEVICES.md, section 6): the tests of that driver use images made here.
Names are kept as they are; the Amiga's characters that a host cannot have in a
name are written as %XX (as the host does).
"""

import argparse
import os
import struct
import sys
import time

BLOCK = 512
BLOCKS = 1760
ROOT = 880
HASH = 72
T_HEADER, T_DATA, T_LIST = 2, 8, 16
ST_ROOT, ST_DIR, ST_FILE = 1, 2, -3
EPOCH = 252460800       # 1978-01-01 00:00:00 UTC
OFS_DATA = BLOCK - 24


class AdfError(Exception):
    pass


def upper(c, intl):
    """AmigaDOS's case folding of one character (code 0-255)."""
    if 97 <= c <= 122:
        return c - 32
    if intl and (224 <= c <= 254) and c != 247:
        return c - 32
    return c


def hash_name(name, intl):
    h = len(name)
    for c in name:
        h = (h * 13 + upper(c, intl)) & 0x7FF
    return h % HASH


def same_name(a, b, intl):
    return len(a) == len(b) and all(upper(x, intl) == upper(y, intl) for x, y in zip(a, b))


def checksum(block, at=20):
    total = 0
    for i in range(0, BLOCK, 4):
        if i != at:
            total += struct.unpack_from('>I', block, i)[0]
    return (-total) & 0xFFFFFFFF


def boot_checksum(data):
    total = 0
    for i in range(0, 1024, 4):
        if i == 4:
            continue
        total += struct.unpack_from('>I', data, i)[0]
        if total > 0xFFFFFFFF:
            total = (total & 0xFFFFFFFF) + 1
    return (~total) & 0xFFFFFFFF


def amiga_date(t):
    days, rest = divmod(max(int(t) - EPOCH, 0), 86400)
    return days, rest // 60, (rest % 60) * 50


def unix_date(days, mins, ticks):
    return EPOCH + days * 86400 + mins * 60 + ticks // 50


def escape(name):
    """A name for a host's folder: the bytes a host cannot have, as %XX."""
    out = []
    for c in name:
        if c in b'/:?*"<>|\\%' or c < 32:
            out.append('%%%02X' % c)
        else:
            out.append(chr(c))
    s = ''.join(out)
    if s.endswith(('.', ' ')):
        s = s[:-1] + '%%%02X' % ord(s[-1])
    return s


def unescape(name):
    out = bytearray()
    i = 0
    while i < len(name):
        if name[i] == '%' and i + 2 < len(name) + 0 and all(h in '0123456789abcdefABCDEF' for h in name[i + 1:i + 3]):
            out.append(int(name[i + 1:i + 3], 16))
            i += 3
        else:
            out += name[i].encode('latin-1', 'replace')
            i += 1
    return bytes(out)


class Entry:
    def __init__(self, name, block, kind, size=0, protect=0, comment=b'', date=0):
        self.name = name        # bytes
        self.block = block
        self.kind = kind        # ST_DIR or ST_FILE
        self.size = size
        self.protect = protect
        self.comment = comment
        self.date = date

    @property
    def is_dir(self):
        return self.kind == ST_DIR


class Adf:
    def __init__(self, data):
        if len(data) != BLOCK * BLOCKS:
            raise AdfError('not a double density image (%d bytes)' % len(data))
        self.data = bytearray(data)
        if self.data[:3] != b'DOS':
            raise AdfError('not an AmigaDOS disk (no DOS boot block)')
        flags = self.data[3]
        self.ffs = bool(flags & 1)
        self.intl = bool(flags & 6)     # international, or with the directory cache
        self.cache = bool(flags & 4)
        root = self.block(ROOT)
        if self.long(root, 0) != T_HEADER or self.slong(root, 508) != ST_ROOT:
            raise AdfError('no root block')
        if checksum(root) != self.long(root, 20):
            raise AdfError('root block checksum')
        self.root = root
        self.label = self.bstr(root, 432, 30)
        self.date = unix_date(*struct.unpack_from('>3I', root, 472))

    # -- raw ---------------------------------------------------------------------
    def block(self, n):
        if not 0 <= n < BLOCKS:
            raise AdfError('block %d outside the disk' % n)
        return self.data[n * BLOCK:(n + 1) * BLOCK]

    @staticmethod
    def long(b, at):
        return struct.unpack_from('>I', b, at)[0]

    @staticmethod
    def slong(b, at):
        return struct.unpack_from('>i', b, at)[0]

    @staticmethod
    def bstr(b, at, most):
        n = min(b[at], most)
        return bytes(b[at + 1:at + 1 + n])

    # -- reading -----------------------------------------------------------------
    def checked(self, n, types):
        b = self.block(n)
        if self.long(b, 0) not in types:
            raise AdfError('block %d: wrong type' % n)
        if checksum(b) != self.long(b, 20):
            raise AdfError('block %d: checksum' % n)
        return b

    def entry_at(self, n):
        b = self.checked(n, (T_HEADER,))
        kind = self.slong(b, 508)
        if kind not in (ST_DIR, ST_FILE):
            raise AdfError('block %d: not a file or a directory' % n)
        return Entry(self.bstr(b, 432, 30), n, kind, self.long(b, 324) if kind == ST_FILE else 0,
                     self.long(b, 320), self.bstr(b, 328, 79), unix_date(*struct.unpack_from('>3I', b, 420)))

    def entries(self, dir_block):
        """The entries of a directory, in the hash table's order (as the disk lists them)."""
        b = self.checked(dir_block, (T_HEADER,))
        out = []
        seen = set()
        for i in range(HASH):
            n = self.long(b, 24 + 4 * i)
            while n:
                if n in seen:
                    raise AdfError('loop in a hash chain of block %d' % dir_block)
                seen.add(n)
                e = self.entry_at(n)
                out.append(e)
                n = self.long(self.block(n), 496)
        return out

    def find(self, dir_block, name):
        for e in self.entries(dir_block):
            if same_name(e.name, name, self.intl):
                return e
        return None

    def lookup(self, path):
        """An entry from a path of the disk ("dir/file"), or None. The root is block 880."""
        here = Entry(self.label, ROOT, ST_DIR)
        for part in [p for p in path.encode('latin-1', 'replace').split(b'/') if p]:
            if not here.is_dir:
                return None
            here = self.find(here.block, part)
            if here is None:
                return None
        return here

    def read(self, entry):
        b = self.checked(entry.block, (T_HEADER,))
        out = bytearray()
        header = b
        while True:
            count = self.long(header, 8)
            blocks = [self.long(header, 24 + 4 * (HASH - 1 - i)) for i in range(count)]
            for n in blocks:
                d = self.block(n)
                if self.ffs:
                    out += d
                else:
                    if self.long(d, 0) != T_DATA or checksum(d) != self.long(d, 20):
                        raise AdfError('data block %d' % n)
                    out += d[24:24 + self.long(d, 12)]
            nxt = self.long(header, 504)
            if not nxt:
                break
            header = self.checked(nxt, (T_LIST,))
            if self.long(header, 4) != nxt:
                raise AdfError('extension block %d: wrong key' % nxt)
        if len(out) < entry.size:
            raise AdfError('%s: shorter than its size' % entry.name.decode('latin-1'))
        return bytes(out[:entry.size])

    def walk(self, dir_block=ROOT, prefix=''):
        for e in self.entries(dir_block):
            path = prefix + e.name.decode('latin-1')
            yield path, e
            if e.is_dir:
                yield from self.walk(e.block, path + '/')

    def free_blocks(self):
        n = 0
        for i in range(25):
            bm = self.long(self.root, 316 + 4 * i)
            if not bm:
                continue
            b = self.block(bm)
            for w in range(1, 128):
                n += bin(self.long(b, 4 * w)).count('1')
        return n


# -- writing ---------------------------------------------------------------------
class Packer:
    """Builds an image from a folder: a contiguous layout (the root in the middle, as the
    Amiga does), the bitmap, the hash tables in name order."""

    def __init__(self, label, ffs, intl):
        self.ffs, self.intl = ffs, intl
        self.data = bytearray(BLOCK * BLOCKS)
        self.used = {0, 1, ROOT, ROOT + 1}
        self.next = ROOT + 2
        self.label = label.encode('latin-1', 'replace')[:30]

    def alloc(self):
        while self.next in self.used:
            self.next += 1
        if self.next >= BLOCKS:
            # the part before the root
            n = 2
            while n in self.used:
                n += 1
            if n >= ROOT:
                raise AdfError('the disk is full')
            self.used.add(n)
            return n
        n = self.next
        self.used.add(n)
        return n

    def put(self, n, b):
        self.data[n * BLOCK:(n + 1) * BLOCK] = b

    @staticmethod
    def setname(b, name, comment=b''):
        b[432] = len(name)
        b[433:433 + len(name)] = name
        b[328] = len(comment)
        b[329:329 + len(comment)] = comment

    def add_dir(self, path, parent, label_for=None):
        """The entries of a host folder under a directory block (the hash table `ht`)."""
        names = sorted(os.listdir(path), key=lambda s: unescape(s))
        table = [0] * HASH
        chain_tail = {}
        for s in names:
            full = os.path.join(path, s)
            name = unescape(s)
            if not 0 < len(name) <= 30:
                raise AdfError('%s: a name of 1 to 30 characters is needed' % s)
            h = hash_name(name, self.intl)
            if os.path.isdir(full):
                n = self.alloc()
                children = self.add_dir(full, n)
                b = bytearray(BLOCK)
                struct.pack_into('>I', b, 0, T_HEADER)
                struct.pack_into('>I', b, 4, n)
                for i, c in enumerate(children):
                    struct.pack_into('>I', b, 24 + 4 * i, c)
                struct.pack_into('>I', b, 320, 0)
                struct.pack_into('>3I', b, 420, *amiga_date(os.path.getmtime(full)))
                self.setname(b, name)
                struct.pack_into('>I', b, 500, parent)
                struct.pack_into('>i', b, 508, ST_DIR)
                kind_block = (n, b)
            else:
                kind_block = self.add_file(full, name, parent)
            n, b = kind_block
            # chain: the new entry goes at the head of the hash slot, as AmigaDOS does
            struct.pack_into('>I', b, 496, table[h])
            table[h] = n
            struct.pack_into('>I', b, 20, 0)
            struct.pack_into('>I', b, 20, checksum(b))
            self.put(n, b)
        return table

    def add_file(self, full, name, parent):
        with open(full, 'rb') as f:
            content = f.read()
        per = BLOCK if self.ffs else OFS_DATA
        count = max((len(content) + per - 1) // per, 0)
        header = self.alloc()
        data_blocks = [self.alloc() for _ in range(count)]
        for i, n in enumerate(data_blocks):
            chunk = content[i * per:(i + 1) * per]
            if self.ffs:
                self.put(n, chunk.ljust(BLOCK, b'\0'))
            else:
                b = bytearray(BLOCK)
                struct.pack_into('>5I', b, 0, T_DATA, header, i + 1, len(chunk),
                                 data_blocks[i + 1] if i + 1 < count else 0)
                b[24:24 + len(chunk)] = chunk
                struct.pack_into('>I', b, 20, checksum(b))
                self.put(n, b)
        # a header holds 72 data blocks, the rest in extension blocks
        groups = [data_blocks[i:i + HASH] for i in range(0, len(data_blocks), HASH)] or [[]]
        blocks = [header] + [self.alloc() for _ in groups[1:]]
        mtime = os.path.getmtime(full)
        for gi, (blk, group) in enumerate(zip(blocks, groups)):
            b = bytearray(BLOCK)
            if gi == 0:
                struct.pack_into('>I', b, 0, T_HEADER)
                struct.pack_into('>I', b, 4, blk)
                struct.pack_into('>I', b, 16, data_blocks[0] if data_blocks else 0)
                struct.pack_into('>I', b, 324, len(content))
                struct.pack_into('>3I', b, 420, *amiga_date(mtime))
                self.setname(b, name)
                struct.pack_into('>i', b, 508, ST_FILE)
            else:
                struct.pack_into('>I', b, 0, T_LIST)
                struct.pack_into('>I', b, 4, blk)
                struct.pack_into('>i', b, 508, ST_FILE)
            struct.pack_into('>I', b, 8, len(group))
            for i, n in enumerate(group):
                struct.pack_into('>I', b, 24 + 4 * (HASH - 1 - i), n)
            struct.pack_into('>I', b, 500, parent if gi == 0 else header)
            if gi + 1 < len(blocks):
                struct.pack_into('>I', b, 504, blocks[gi + 1])
            if gi == 0:
                kind_block = (blk, b)
            else:
                struct.pack_into('>I', b, 20, checksum(b))
                self.put(blk, b)
        return kind_block

    def finish(self, folder):
        table = self.add_dir(folder, ROOT)
        r = bytearray(BLOCK)
        struct.pack_into('>I', r, 0, T_HEADER)
        struct.pack_into('>I', r, 12, HASH)
        for i, c in enumerate(table):
            struct.pack_into('>I', r, 24 + 4 * i, c)
        struct.pack_into('>i', r, 312, -1)          # the bitmap is valid
        struct.pack_into('>I', r, 316, ROOT + 1)
        now = amiga_date(time.time())
        struct.pack_into('>3I', r, 420, *now)
        r[432] = len(self.label)
        r[433:433 + len(self.label)] = self.label
        struct.pack_into('>3I', r, 472, *now)
        struct.pack_into('>3I', r, 484, *now)
        struct.pack_into('>i', r, 508, ST_ROOT)
        struct.pack_into('>I', r, 20, checksum(r))
        self.put(ROOT, r)
        bm = bytearray(BLOCK)
        for n in range(2, BLOCKS):
            if n not in self.used:
                bit = n - 2
                w = 1 + bit // 32
                v = struct.unpack_from('>I', bm, 4 * w)[0] | (1 << (bit % 32))
                struct.pack_into('>I', bm, 4 * w, v)
        struct.pack_into('>I', bm, 0, checksum(bm, 0))
        self.put(ROOT + 1, bm)
        flags = (1 if self.ffs else 0) | (2 if self.intl else 0)
        self.data[0:4] = b'DOS' + bytes([flags])
        struct.pack_into('>I', self.data, 8, ROOT)
        struct.pack_into('>I', self.data, 4, boot_checksum(self.data))
        return bytes(self.data)


def pack(folder, label=None, ffs=True, intl=False):
    return Packer(label or os.path.basename(os.path.abspath(folder)) or 'Disk', ffs, intl).finish(folder)


# -- the command line ---------------------------------------------------------------
def protect_text(p):
    # the Amiga's bits: set means *denied* for rwed; hspa (and the archive bit) set means allowed
    out = ''
    for bit, ch in ((7, 'h'), (6, 's'), (5, 'p'), (4, 'a')):
        out += ch if p & (1 << bit) else '-'
    for bit, ch in ((2, 'r'), (3, 'w'), (0, 'e'), (1, 'd')):
        out += '-' if p & (1 << bit) else ch
    return out


def command_line(argv):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('info'); s.add_argument('image')
    s = sub.add_parser('list'); s.add_argument('-l', action='store_true'); s.add_argument('image'); s.add_argument('path', nargs='?', default='')
    s = sub.add_parser('extract'); s.add_argument('image'); s.add_argument('folder'); s.add_argument('path', nargs='?', default='')
    s = sub.add_parser('pack'); s.add_argument('folder'); s.add_argument('image')
    s.add_argument('--label'); s.add_argument('--ofs', action='store_true'); s.add_argument('--intl', action='store_true')
    a = ap.parse_args(argv)
    try:
        if a.cmd == 'pack':
            with open(a.image, 'wb') as f:
                f.write(pack(a.folder, a.label, not a.ofs, a.intl))
            return 0
        with open(a.image, 'rb') as f:
            disk = Adf(f.read())
        if a.cmd == 'info':
            files = sum(1 for _, e in disk.walk() if not e.is_dir)
            print('%s: %s%s, %d files, %d blocks free' % (
                disk.label.decode('latin-1'), 'FFS' if disk.ffs else 'OFS',
                ' international' if disk.intl else '', files, disk.free_blocks()))
        elif a.cmd == 'list':
            base = disk.lookup(a.path)
            if base is None or not base.is_dir:
                raise AdfError('%s: not a directory' % a.path)
            for path, e in disk.walk(base.block, a.path.strip('/') + '/' if a.path.strip('/') else ''):
                if a.l:
                    print('%s %8s %s%s' % (protect_text(e.protect), 'dir' if e.is_dir else e.size, path, '/' if e.is_dir else ''))
                else:
                    print(path + ('/' if e.is_dir else ''))
        else:
            start = disk.lookup(a.path)
            if start is None:
                raise AdfError('%s: not found' % a.path)
            os.makedirs(a.folder, exist_ok=True)
            if not start.is_dir:
                with open(os.path.join(a.folder, escape(start.name)), 'wb') as f:
                    f.write(disk.read(start))
                return 0
            def dump(block, to):
                for e in disk.entries(block):
                    target = os.path.join(to, escape(e.name))
                    if e.is_dir:
                        os.makedirs(target, exist_ok=True)
                        dump(e.block, target)
                    else:
                        with open(target, 'wb') as f:
                            f.write(disk.read(e))
                        os.utime(target, (e.date, e.date))
            dump(start.block, a.folder)
    except (AdfError, OSError) as e:
        print('adf.py: %s' % e, file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(command_line(sys.argv[1:]))
