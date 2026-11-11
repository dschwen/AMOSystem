#!/usr/bin/env python3
"""Host-side AMOS Professional tokenizer and detokenizer.

A port of the editor's Tokenise/Detok routines (+Edit.s). Token values are
byte offsets into the token tables of the libraries, which are read from the
assembled .Lib files, so the output is exactly what the AMOS editor would
produce.

Command line:
  amostok.py detok  PROGRAM.AMOS            print the listing
  amostok.py tok    LISTING.asc OUT.AMOS    tokenize a text listing
  amostok.py check  PROGRAM.AMOS...         detokenize + retokenize round trip

Libraries are looked up in --libs (default: build/, then AMOS/APSystem/).
"""

import argparse
import math
import os
import struct
import sys

# Special tokens (+Equ.s)
TK_VAR, TK_LAB, TK_PRO, TK_LGO = 0x06, 0x0C, 0x12, 0x18
TK_BIN, TK_CH1, TK_CH2, TK_HEX, TK_ENT, TK_FL = 0x1E, 0x26, 0x2E, 0x36, 0x3E, 0x46
TK_EXT, TK_PAR1 = 0x4E, 0x74
TK_FOR, TK_RPT, TK_WHL, TK_DO = 0x23C, 0x250, 0x268, 0x27E
TK_EXIF, TK_EXIT, TK_IF, TK_THEN, TK_ELSE = 0x290, 0x29E, 0x2BE, 0x2C6, 0x2D0
TK_ON, TK_PROC, TK_DATA = 0x316, 0x376, 0x404
TK_HPR, TK_PR = 0x46A, 0x476
TK_REM1, TK_REM2 = 0x64A, 0x652
TK_ELSI, TK_EQU, TK_STRUS, TK_DFL, TK_DPRE = 0x25A4, 0x2A40, 0x2A64, 0x2B6A, 0x2B84

# Extension slot -> library file (+Interpreter_Config.s, messages 16..41)
EXTENSIONS = {1: 'AMOSPro_Music.Lib', 2: 'AMOSPro_Compact.Lib', 3: 'AMOSPro_Request.Lib',
              5: 'AMOSPro_Compiler.Lib', 6: 'AMOSPro_IOPorts.Lib',
              # (of other authors, if the file is there: their default slots)
              8: 'AMOSPro_AMCAF.Lib', 12: 'AMOSPro_TURBO_Plus.Lib'}

# Dtk_Operateurs (+Edit.s): names exactly as in the table
OPERATORS = [b' xor ', b' or ', b' and ', b'<>', b'><', b'<=', b'=<', b'>=', b'=>',
             b'=', b'<', b'>', b'+', b'-', b' mod ', b'*', b'/', b'^']
OPERATOR_PARAMS = {b'+': b'O22', b'-': b'O22', b'=': b'O20', b'<': b'O20', b'>': b'O20',
                   b'<>': b'O20', b'><': b'O20', b'<=': b'O20', b'=<': b'O20',
                   b'>=': b'O20', b'=>': b'O20'}


def lower(c):
    return c + 32 if 0x41 <= c <= 0x5A else c


def is_letter(c):
    return 0x61 <= c <= 0x7A


# ---- token tables -----------------------------------------------------------

class Entry:
    def __init__(self, offset, name, params, table):
        self.offset = offset        # token value (offset in the table)
        self.name = name            # raw name bytes, last one with bit 7 set
        self.params = params
        self.table = table


class TokenTable:
    """One library's token table: raw bytes plus the entry list."""

    def __init__(self, data, start=0):
        self.data = data            # bytes; offset 0 is the table base (AdTokens)
        self.entries = []
        self.by_offset = {}
        self.fast = {}              # letter -> [Entry] (Tok_Init's fast tables)
        pos = start
        while struct.unpack_from('>H', data, pos)[0] != 0:
            name_start = pos + 4
            p = name_start
            while data[p] < 0x80:
                p += 1
            name = data[name_start:p + 1]
            p += 1
            params_start = p
            while data[p] < 0x80:
                p += 1
            entry = Entry(pos, name, data[params_start:p], self)
            # parameter variants (name $80..$9F, or the same name repeated)
            # belong to the entry before
            prev = self.entries[-1].base if self.entries else None
            entry.base = prev if prev and (0x80 <= name[0] <= 0x9F or name == prev.name) \
                else entry
            self.entries.append(entry)
            self.by_offset[pos] = entry
            p += 1
            pos = p + (p & 1)
            # first significant letter, skipping "!" and " "
            for c in name:
                c &= 0x7F
                if c not in (0x21, 0x20):
                    c = lower(c)
                    if is_letter(c):
                        self.fast.setdefault(c, []).append(entry)
                    break

    @classmethod
    def from_lib(cls, path):
        with open(path, 'rb') as f:
            raw = f.read()
        body = raw[0x20:]                       # skip the hunk header
        tk_offset = struct.unpack_from('>L', body, 0)[0]
        # C_Tk (after 4 longs, a word, "AP20"; the old form, of the extensions of other authors, has
        # no "AP20")
        base = (22 if body[18:22] == b'AP20' else 18) + tk_offset
        # AdTokens points at C_Tk; slow searches start after the first entry
        table = cls(body[base:], 6)
        return table


def operator_table():
    """Build Dtk_Operateurs; token values are negative offsets from Dtk_OpFin."""
    blob = bytearray()
    offsets = []
    for name in OPERATORS:
        offsets.append(len(blob))
        blob += b'\x00\x01\x00\x01' + name[:-1] + bytes([name[-1] | 0x80])
        blob += OPERATOR_PARAMS.get(name, b'O00') + b'\xff'
        if len(blob) & 1:
            blob += b'\x00'
    end = len(blob)
    table = TokenTable(bytes(blob) + b'\x00\x00\x00\x00', 0)
    for entry, off in zip(table.entries, offsets):
        entry.offset = off - end
    table.by_offset = {e.offset: e for e in table.entries}
    return table


class Tables:
    def __init__(self, libdirs, extensions=None):
        """extensions: {slot: file} for the extensions of other authors in other slots than their
        default ones (the file's own default entry goes; an empty file frees a slot)"""
        def find(name):
            for d in libdirs:
                p = os.path.join(d, name)
                if os.path.exists(p):
                    return p
            return None

        main = find('AMOSPro.Lib')
        if main is None:
            sys.exit('AMOSPro.Lib not found in ' + ', '.join(libdirs))
        self.main = TokenTable.from_lib(main)
        self.ops = operator_table()
        self.ext = {}
        mapping = dict(EXTENSIONS)
        for slot, name in (extensions or {}).items():
            for other in [k for k, v in mapping.items() if name and v.lower() == name.lower()]:
                del mapping[other]
            mapping.pop(slot, None)
            if name:
                mapping[slot] = name
        for slot, name in mapping.items():
            p = find(name)
            if p:
                self.ext[slot] = TokenTable.from_lib(p)

    def libraries(self):
        """AdTokens order: main library, then extensions 1..25."""
        yield 0, self.main
        for slot in sorted(self.ext):
            yield slot, self.ext[slot]


# ---- numbers ----------------------------------------------------------------

def ffp_encode(value):
    """Motorola Fast Floating Point, round to nearest."""
    if value == 0:
        return 0
    sign = 0x80 if value < 0 else 0
    m, e = math.frexp(abs(value))           # value = m * 2**e, 0.5 <= m < 1
    mant = int(round(m * (1 << 24)))
    if mant == 1 << 24:
        mant >>= 1
        e += 1
    exp = e + 64
    if exp <= 0:
        return 0
    if exp > 127:
        return 0xFFFFFF7F | sign
    return (mant << 8) | sign | exp


def ffp_decode(bits):
    if bits & 0xFFFFFF00 == 0:
        return 0.0
    mant = bits >> 8
    value = mant / (1 << 24) * 2.0 ** ((bits & 0x7F) - 64)
    return -value if bits & 0x80 else value


def format_float(value, encode):
    for digits in range(1, 18):
        text = f'{value:.{digits}g}'
        try:
            if encode(float(text)) == encode(value):
                break
        except OverflowError:
            pass
    if 'e' in text:
        mant, exp = text.split('e')
        text = f'{mant}E{exp[0]}{exp[1:].lstrip("0") or "0"}'
    if '.' not in text and 'E' not in text:
        text += '.0'
    return text


def val_rout(line, pos, double):
    """ValRout without sign handling. Returns (token, value bytes, new pos)."""
    n = len(line)

    def at(i):
        return line[i] if i < n else 0

    i = pos
    while at(i) == 0x20:
        i += 1
    c = at(i)
    if c == 0:
        return None
    if c == ord('$'):
        return int_parse(line, i + 1, 16, TK_HEX)
    if c == ord('%'):
        return int_parse(line, i + 1, 2, TK_BIN)
    if not (c == ord('.') or 0x30 <= c <= 0x39):
        return None
    # decimal: integer or float?
    start = i
    j = i
    flags = 0
    while True:
        c = at(j)
        j += 1
        if c == 0:
            break
        if c == 0x20 or 0x30 <= c <= 0x39:
            continue
        if c == ord('.'):
            if flags & 1:
                break
            flags |= 1
            continue
        if c in (ord('e'), ord('E')):
            c = at(j)
            j += 1
            while c == 0x20:
                c = at(j)
                j += 1
            if c in (ord('+'), ord('-')):
                flags |= 2
                c = at(j)
                j += 1
                while c == 0x20:
                    c = at(j)
                    j += 1
            while 0x30 <= c <= 0x39:
                flags |= 2
                c = at(j)
                j += 1
                while c == 0x20:
                    c = at(j)
                    j += 1
            break
        break
    end = j - 1
    if flags == 0:
        return int_parse(line, start, 10, TK_ENT)
    text = bytes(c for c in line[start:end] if c != 0x20).decode('latin-1').lower()
    try:
        value = float(text)
    except ValueError:
        value = float(text.rstrip('e+-') or 0)
    if double:
        return TK_DFL, struct.pack('>d', value), end
    return TK_FL, struct.pack('>L', ffp_encode(value)), end


def int_parse(line, i, base, token):
    n = len(line)
    value = 0
    digits = 0
    limit = {16: 8, 2: 32, 10: None}[base]
    while i < n:
        c = line[i]
        if base == 10 and c == 0x20:
            i += 1
            continue
        if base == 2 and c == 0x20:
            i += 1
            continue
        d = c - 0x20 if 0x61 <= c <= 0x7A else c
        d -= 0x30
        if base == 16 and d >= 10:
            if d < 17:
                break
            d -= 7
        if not 0 <= d < base:
            break
        value = value * base + d
        digits += 1
        if base == 10 and value > 0x7FFFFFFF:
            return None
        if limit and digits > limit:
            return None
        i += 1
    if digits == 0:
        return None if base == 10 else (token, struct.pack('>L', 0), i)
    return token, struct.pack('>L', value & 0xFFFFFFFF), i


# ---- tokenizer ---------------------------------------------------------------

class Tokenizer:
    def __init__(self, tables):
        self.t = tables
        self.double = False

    def match(self, entry, line, pos, skip_first):
        """Compare the rest of an entry name with the source (TkRe0..TkKt).

        Returns the new source position or None."""
        name = entry.name
        k = 1 if skip_first else 0
        # skip leading "!" / " " (+ the first letter, already compared)
        while k < len(name) and (name[k] & 0x7F) in (0x21, 0x20) and name[k] < 0x80:
            k += 1
        if name[k] >= 0x80:
            # the first letter is also the last one (Tkl4)
            return pos
        k += 1                                  # first letter matched by caller
        a = pos
        n = len(line)
        while True:
            d0 = lower(line[a]) if a < n else 0
            a += 1
            while True:
                if k >= len(name):
                    return None
                d1 = name[k]
                k += 1
                if d1 >= 0x80:
                    # last character of the name
                    a -= 1
                    d1 &= 0x7F
                    if d1 == 0x20:
                        return a
                    if d0 != d1:
                        return None
                    return a + 1
                if d1 == 0x20:
                    if d0 == 0x20:
                        break           # both advance
                    continue            # optional space in the name
                if d0 != d1:
                    return None
                break

    def find_keyword(self, line, pos):
        """pos points just after the first character. Returns (kind, entry, newpos)."""
        first = lower(line[pos - 1])
        if first == ord('!'):
            return None
        if first == ord('?'):
            i = pos
            while i < len(line) and line[i] == 0x20:
                i += 1
            if i < len(line) and line[i] == ord('#'):
                return ('main', TK_HPR, i + 1)
            return ('main', TK_PR, pos)
        # operators first, slow search, first match wins
        for entry in self.t.ops.entries:
            if self.first_char(entry) == first:
                newpos = self.match(entry, line, pos, False)
                if newpos is not None:
                    return ('op', entry.offset, newpos)
        if is_letter(first):
            best = None
            for slot, table in self.t.libraries():
                for entry in table.fast.get(first, []):
                    newpos = self.match(entry, line, pos, False)
                    if newpos is None:
                        continue
                    length = self.match_length(entry, line, pos)
                    if best is None or length > best[0]:
                        best = (length, slot, entry, newpos)
            if best:
                _, slot, entry, newpos = best
                return ('main' if slot == 0 else slot, entry.offset, newpos)
            return None
        for slot, table in self.t.libraries():
            for entry in table.entries[1:] if slot == 0 else table.entries:
                if self.first_char(entry) == first:
                    newpos = self.match(entry, line, pos, False)
                    if newpos is not None:
                        return ('main' if slot == 0 else slot, entry.offset, newpos)
        return None

    @staticmethod
    def first_char(entry):
        for c in entry.name:
            if c >= 0x80:
                return lower(c & 0x7F) if (c & 0x7F) not in (0x20,) else None
            if c not in (0x21, 0x20):
                return lower(c)
        return None

    def match_length(self, entry, line, pos):
        """Bytes of the entry consumed up to its last name character (a1-d4)."""
        return 4 + len(entry.name) - 1

    def tokenize_line(self, text):
        line = text.encode('latin-1') if isinstance(text, str) else bytes(text)
        line = line.rstrip(b'\r\n')
        out = bytearray(2)
        n = len(line)
        # indentation
        i = 0
        while i < n and line[i] == 0x20:
            i += 1
        if i >= n:
            return b'\x02\x00\x00\x00'           # empty line (TokVide)
        out[1] = min(i + 1, 127)
        flags_var = flags_lab = False
        line_start = True
        cur = None                  # start of the token being built
        if 0x30 <= line[i] <= 0x39:
            flags_var = True
            flags_lab = True
            cur = len(out)
            out += struct.pack('>HL', TK_VAR, 0)[:6]
            out += bytes([line[i]])
            i += 1
        elif line[i] == ord("'"):
            i += 1
            out += struct.pack('>H', TK_REM2)
            return self.finish_rem(out, line, i)

        string_char = None
        while True:
            if len(out) > 512:
                break
            if i >= n:
                break
            c = line[i]
            i += 1
            if string_char is not None:
                if c == string_char:
                    string_char = None
                    self.end_string(out, cur)
                    continue
                out.append(c)
                continue
            if flags_var:
                c = lower(c)
                ended, i, line_start = self.var_char(out, cur, c, i, flags_lab, line_start)
                if ended:
                    flags_var = flags_lab = False
                continue
            if c in (ord('"'), ord("'")):
                string_char = c
                cur = len(out)
                out += struct.pack('>HH', TK_CH1 if c == ord('"') else TK_CH2, 0)
                continue
            if c == 0x20:
                continue
            num = val_rout(line, i - 1, self.double)
            if num is not None:
                token, value, i = num
                out += struct.pack('>H', token) + value
                continue
            found = self.find_keyword(line, i)
            if found is None:
                # start of a variable
                c = lower(c)
                if c == ord('_') or c >= 128 or is_letter(c):
                    cur = len(out)
                    out += struct.pack('>HL', TK_VAR, 0) + bytes([c])
                    flags_var = True
                continue
            kind, token, i = found
            line_start = False
            if kind not in ('main', 'op'):
                table = self.t.ext[kind]
                out += struct.pack('>HBBH', TK_EXT, kind, 0, token)
                continue
            token &= 0xFFFF
            out += struct.pack('>H', token)
            if TK_EQU <= token <= TK_STRUS:
                out += bytes(6)
            elif token == TK_ON:
                out += bytes(4)
            elif token in (TK_DATA, TK_FOR, TK_RPT, TK_WHL, TK_DO, TK_IF, TK_ELSI):
                out += bytes(2)
            elif token == TK_REM1:
                return self.finish_rem(out, line, i)
            elif token in (TK_EXIT, TK_EXIF):
                out += bytes(4)
            elif token == TK_PROC:
                out += bytes(8)
            elif token == TK_DPRE:
                self.double = True
            elif token in (TK_ELSE, TK_THEN):
                if token == TK_ELSE:
                    out += bytes(2)
                j = i
                while j < n and line[j] == 0x20:
                    j += 1
                if j < n and 0x30 <= line[j] <= 0x39:
                    i = j + 1
                    cur = len(out)
                    out += struct.pack('>HL', TK_LGO, 0) + bytes([line[j]])
                    flags_var = True
        # end of line
        if flags_var:
            self.var_char(out, cur, 0, i, flags_lab, line_start, eol=True)
        elif string_char is not None:
            self.end_string(out, cur)
        return self.finish(out)

    def var_char(self, out, cur, c, i, flags_lab, line_start, eol=False):
        """One character of a variable / label (TkVD..TkV8)."""
        flag = 0
        if flags_lab:
            if not eol and 0x30 <= c <= 0x39:
                out.append(c)
                return False, i, line_start
            if c != ord(':') and not eol:
                i -= 1
            struct.pack_into('>H', out, cur, TK_LAB)
            return self.end_var(out, cur, 0), i, False
        if not eol and (c == ord('_') or 0x30 <= c <= 0x39 or is_letter(c) or c > 128):
            out.append(c)
            return False, i, line_start
        if line_start and c == ord(':') and not eol:
            struct.pack_into('>H', out, cur, TK_LAB)
            return self.end_var(out, cur, 0), i, False
        if not eol:
            i -= 1
            if c == ord('$'):
                flag = 2
                i += 1
            elif c == ord('#'):
                flag = 1
                i += 1
        return self.end_var(out, cur, flag), i, False

    @staticmethod
    def end_var(out, cur, flag):
        if len(out) & 1:
            out.append(0)
        out[cur + 4] = len(out) - cur - 6
        out[cur + 5] = flag
        return True

    @staticmethod
    def end_string(out, cur):
        # the length is taken before the padding byte (TkC1)
        struct.pack_into('>H', out, cur + 2, len(out) - cur - 4)
        if len(out) & 1:
            out.append(0)

    def finish_rem(self, out, line, i):
        out += b'\x00\x00'
        start = len(out)
        out += line[i:]
        if len(out) & 1:
            out.append(0x20)
        struct.pack_into('>H', out, start - 2, len(out) - start)
        return self.finish(out)

    @staticmethod
    def finish(out):
        out += b'\x00\x00'
        if len(out) >= 510:
            raise ValueError('line too long')
        out[0] = len(out) // 2
        return bytes(out)


# ---- detokenizer -----------------------------------------------------------------

def token_size(token, data, pos):
    """TInst: size of the token's inline data after the token word."""
    if token <= TK_LGO:
        return 4 + data[pos + 2]
    if token in (TK_CH1, TK_CH2, TK_REM1, TK_REM2):
        length = struct.unpack_from('>H', data, pos)[0]
        return 2 + length + (length & 1)
    if token == TK_DFL:
        return 8
    if token <= TK_FL or token == TK_EXT:
        return 4
    if token in (TK_FOR, TK_RPT, TK_WHL, TK_DO, TK_IF, TK_ELSE, TK_ELSI, TK_DATA):
        return 2
    if token == TK_PROC:
        return 8
    if token in (TK_EXIT, TK_EXIF, TK_ON):
        return 4
    if TK_EQU <= token <= TK_STRUS:
        return 6
    return 0


def name_text(entry):
    """The keyword as written (Dtk4..Dtk5), resolving "same as previous" names."""
    table = entry.table
    name = entry.name
    if 0x80 <= name[0] <= 0x9F:
        # variant: the name of an earlier entry
        idx = table.entries.index(entry)
        while idx > 0 and 0x80 <= table.entries[idx].name[0] <= 0x9F:
            idx -= 1
        name = table.entries[idx].name
    if name[0] == ord('!'):
        name = name[1:]
    text = bytes(c & 0x7F for c in name).decode('latin-1')
    return text


def detokenize_line(data, tables):
    """data: one line including its 2-byte header."""
    out = ''
    indent = data[1]
    if data[0] == 0:
        return ''
    out = ' ' * max(indent - 1, 0)
    start_len = len(out)
    pos = 2
    after_var = False
    while True:
        token = struct.unpack_from('>H', data, pos)[0]
        pos += 2
        if token == 0:
            break
        if token <= TK_LGO:
            if after_var and not out.endswith(' '):
                out += ' '
            length, flag = data[pos + 2], data[pos + 3]
            name = data[pos + 4:pos + 4 + length].split(b'\x00')[0].decode('latin-1')
            out += name
            if token == TK_LAB and not name[:1].isdigit():
                out += ':'
            elif token == TK_VAR:
                out += {1: '#', 2: '$'}.get(flag & 3, '')
            pos += 4 + length
            after_var = True
            continue
        if token < TK_EXT or token == TK_DFL:
            if after_var and not out.endswith(' '):
                out += ' '
            after_var = False
            out += constant_text(token, data, pos)
            pos += token_size(token, data, pos)
            continue
        after_var = False
        signed = token - 0x10000 if token & 0x8000 else token
        if token == TK_PAR1:
            if len(out) > start_len and out.endswith(' '):
                out = out[:-1]
            out += '('
            continue
        if token == TK_EXT:
            slot = data[pos]
            offset = struct.unpack_from('>H', data, pos + 2)[0]
            table = tables.ext.get(slot)
            entry = table.by_offset.get(offset) if table else None
            if entry is None:
                raise KeyError(f'unknown extension token {slot}:{offset:#x}')
        elif signed < 0:
            entry = tables.ops.by_offset[signed]
        else:
            entry = tables.main.by_offset.get(token)
            if entry is None:
                raise KeyError(f'unknown token {token:#x}')
        kind = entry.params[:1]
        word = name_text(entry)
        if not (kind in (b'O', b'V') or (kind and 0x30 <= kind[0] <= 0x38)):
            if len(out) > start_len and not out.endswith(' '):
                out += ' '
        out += word
        if token in (TK_REM1, TK_REM2):
            length = struct.unpack_from('>H', data, pos)[0]
            out += data[pos + 2:pos + 2 + length].split(b'\x00')[0].decode('latin-1')
            pos += 2 + length + (length & 1)
            continue
        if kind == b'I':
            out += ' '
        pos += token_size(token, data, pos)
    return out


def constant_text(token, data, pos):
    if token in (TK_CH1, TK_CH2):
        length = struct.unpack_from('>H', data, pos)[0]
        quote = '"' if token == TK_CH1 else "'"
        return quote + data[pos + 2:pos + 2 + length].decode('latin-1') + quote
    value = struct.unpack_from('>L', data, pos)[0]
    if token == TK_ENT:
        return str(value - (1 << 32) if value & 0x80000000 else value)
    if token == TK_HEX:
        return f'${value:X}'
    if token == TK_BIN:
        return f'%{value:b}'
    if token == TK_FL:
        return format_float(ffp_decode(value), ffp_encode)
    if token == TK_DFL:
        d = struct.unpack_from('>d', data, pos)[0]
        return format_float(d, lambda v: struct.pack('>d', v))
    raise KeyError(f'unknown constant token {token:#x}')


# ---- program files -----------------------------------------------------------------

HEADER_PRO = b'AMOS Pro101v\0\0\0\0'


class Program:
    def __init__(self, header=HEADER_PRO, lines=None, banks=b'AmBs\x00\x00'):
        self.header = header
        self.lines = lines or []      # raw tokenized lines
        self.banks = banks

    @classmethod
    def load(cls, path):
        with open(path, 'rb') as f:
            data = f.read()
        header = data[:16]
        size = struct.unpack_from('>L', data, 16)[0]
        code = data[20:20 + size]
        lines = []
        pos = 0
        while pos < len(code):
            length = code[pos] * 2
            if length == 0:
                break
            lines.append(code[pos:pos + length])
            pos += length
        return cls(header, lines, data[20 + size:])

    def save(self, path):
        code = b''.join(self.lines)
        with open(path, 'wb') as f:
            f.write(self.header + struct.pack('>L', len(code)) + code + self.banks)


def tokenize_program(text, tables):
    tok = Tokenizer(tables)
    lines = []
    for line in text.splitlines():
        lines.append(tok.tokenize_line(line))
    header = bytearray(HEADER_PRO)
    if tok.double:
        header[15] = 0x83
    return Program(bytes(header), lines)


# ---- command line -----------------------------------------------------------------

def default_libdirs():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    return [os.path.join(here, 'build'), os.path.join(here, 'AMOS', 'APSystem')]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--libs', action='append', help='directory with the .Lib files')
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('detok')
    p.add_argument('program')
    p = sub.add_parser('tok')
    p.add_argument('listing')
    p.add_argument('output')
    p = sub.add_parser('check')
    p.add_argument('programs', nargs='+')
    p.add_argument('-v', action='store_true', help='show every differing line')
    args = ap.parse_args()
    tables = Tables(args.libs or default_libdirs())

    if args.cmd == 'detok':
        prog = Program.load(args.program)
        for line in prog.lines:
            sys.stdout.write(detokenize_line(line, tables) + '\n')
    elif args.cmd == 'tok':
        with open(args.listing, encoding='latin-1') as f:
            prog = tokenize_program(f.read(), tables)
        prog.save(args.output)
    elif args.cmd == 'check':
        total_bad = 0
        for path in args.programs:
            prog = Program.load(path)
            tok = Tokenizer(tables)
            tok.double = bool(prog.header[15] & 0x80) if len(prog.header) >= 16 else False
            bad = 0
            for line in prog.lines:
                try:
                    text = detokenize_line(line, tables)
                    again = tok.tokenize_line(text)
                except Exception as e:      # noqa: BLE001 - report and continue
                    text, again = f'<{e}>', b''
                if normalise(again, tables) != normalise(line, tables):
                    bad += 1
                    if args.v or bad <= 3:
                        print(f'{path}: {text!r}\n   orig {line.hex()}\n   new  {again.hex()}')
            total_bad += bad
            print(f'{path}: {len(prog.lines)} lines, {bad} differ')
        sys.exit(1 if total_bad else 0)


def normalise(line, tables):
    """Undo what the verifier and the editor change in a tokenized line, so
    that tested and untested programs compare equal: variable offsets and
    flags, procedure calls, structure distances, parameter-count variants of
    instructions and the indentation of empty lines."""
    if not line:
        return line
    out = bytearray(line)
    if len(out) == 4:
        out[1] = 0
    pos = 2
    while pos + 1 < len(out):
        token = struct.unpack_from('>H', out, pos)[0]
        if token in (TK_PRO, TK_LGO):
            token = TK_VAR
            struct.pack_into('>H', out, pos, token)
        elif TK_EXT < token < 0x8000 and token in tables.main.by_offset:
            struct.pack_into('>H', out, pos, tables.main.by_offset[token].base.offset)
        pos += 2
        if token == 0:
            break
        if token <= TK_LGO:
            out[pos:pos + 2] = b'\0\0'          # variable offset
            if token == TK_VAR:
                out[pos + 3] &= 0x03            # keep only the type
        elif token in (TK_FOR, TK_RPT, TK_WHL, TK_DO, TK_IF, TK_ELSE, TK_ELSI, TK_DATA,
                       TK_EXIT, TK_EXIF, TK_ON, TK_PROC) or TK_EQU <= token <= TK_STRUS:
            size = token_size(token, out, pos)
            out[pos:pos + size] = bytes(size)
        elif token == TK_EXT:
            out[pos + 1] = 0                    # parameter count
            table = tables.ext.get(out[pos])
            offset = struct.unpack_from('>H', out, pos + 2)[0]
            if table and offset in table.by_offset:
                struct.pack_into('>H', out, pos + 2, table.by_offset[offset].base.offset)
        pos += token_size(token, out, pos)
    return bytes(out)


if __name__ == '__main__':
    main()
