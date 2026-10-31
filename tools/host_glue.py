#!/usr/bin/env python3
"""The assembler's labels, tables and glue for a build on another host.

  host_glue.py --out host_glue.cpp [--host-dir src/host] [--equ FILE]...
               [--image LABEL=FILE]... -- ASM...

On the Amiga, the C++ refers to labels of the assembler: entries of glue,
tables of entries, data. Another host has no 68000. This lays out each
source's data and tables as the assembler does (each source one object,
its labels at their offsets; those the C++ names are symbols there), and
puts a small slot in place of each entry, which src/host/glue.cpp knows
by its address (amos::host::GlueEntry): the glue macro it was made with
(src/glue.i, src/w/wglue.i...) and the C++ function behind it. The bytes
in front of the entries ("GetP", the LibCall tag) are there as on the
Amiga, for the loader and the ChrGet that read them.

Entries made of other assembler are "hand" entries: src/host has a
function host_<label>(AllRegs &) for each it needs; the others are
"missing" (the host stops if one is called). Labels are local to their
source unless exported (xdef). --image gives the binary of a library image
(vasm -Fbin) for the label of its Include; --equ files of equates, as
+LEqu.s.
"""

import argparse
import os
import re
import sys

ap = argparse.ArgumentParser()
ap.add_argument('--out', required=True)
ap.add_argument('--host-dir', default='src/host')
ap.add_argument('--image', action='append', default=[])
ap.add_argument('--equ', action='append', default=[])
ap.add_argument('--include', action='append', default=['.', 'build/gen', 'includes', 'extensions', 'src'])
ap.add_argument('--list-missing', action='store_true')
ap.add_argument('--cxx', action='append', default=[], help='NAME=EXPR: an equate the C++ knows')
ap.add_argument('sources', nargs='+')
args = ap.parse_args()

images = dict(i.split('=', 1) for i in args.image)

# ---- the hand entries src/host has --------------------------------------
hand_impl = set()
defined = set()         # the assembler's functions src/host defines (amos_call...)
for root, _, files in os.walk(args.host_dir):
    for f in files:
        if f.endswith('.cpp'):
            text = open(os.path.join(root, f)).read()
            for m in re.finditer(r'\bvoid\s+host_(\w+)\s*\(\s*(?:amos::)?AllRegs\s*&', text):
                hand_impl.add(m.group(1))
            for m in re.finditer(r'^(?:\[\[noreturn\]\]\s*)?[A-Za-z_][\w:]*[\s*&]+(\w+)(?:\[\w*\])?\s*[(;=\[]', text, re.M):
                defined.add(m.group(1))

# ---- lines --------------------------------------------------------------
def strip_comment(line):
    line = line.rstrip('\n').rstrip('\r')
    if line.lstrip().startswith('*') or line.startswith(';'):
        return ''
    out, q = [], None
    for c in line:
        if q:
            out.append(c)
            if c == q:
                q = None
            continue
        if c in '"\'':
            q = c
        elif c == ';':
            break
        out.append(c)
    return ''.join(out)

def fields(code):
    """the label, the opcode and the operands of a line (Devpac: a label
    in column 1; anything after the operands is a comment)"""
    label = None
    rest = code
    if code and code[0] not in ' \t':
        m = re.match(r'^([A-Za-z_.][\w.]*):?', code)
        if not m:
            return None, None, ''
        label = m.group(1)
        rest = code[m.end():]
    rest = rest.strip()
    m = re.match(r'^([A-Za-z_.][\w.]*)', rest)
    if not m:
        return label, None, ''
    op = m.group(1)
    tail = rest[m.end():].lstrip()
    out, q = [], None
    for c in tail:
        if q:
            out.append(c)
            if c == q:
                q = None
            continue
        if c in '"\'':
            q = c
            out.append(c)
            continue
        if c in ' \t':
            break
        out.append(c)
    return label, op, ''.join(out)

def split_operands(s):
    out, cur, q, depth = [], [], None, 0
    for c in s:
        if q:
            cur.append(c)
            if c == q:
                q = None
            continue
        if c in '"\'':
            q = c
            cur.append(c)
            continue
        if c == '(':
            depth += 1
        if c == ')':
            depth -= 1
        if c == ',' and depth == 0:
            out.append(''.join(cur))
            cur = []
            continue
        cur.append(c)
    out.append(''.join(cur))
    return [o.strip() for o in out]

# ---- equates ------------------------------------------------------------
equ = {}

def tokens(s):
    """the tokens of an expression: ('n', value), ('s', symbol), ('o', op)"""
    i = 0
    s = s.strip()
    while i < len(s):
        c = s[i]
        if c.isspace():
            i += 1
        elif c == '$':
            m = re.match(r'\$([0-9A-Fa-f]+)', s[i:])
            yield ('n', int(m.group(1), 16))
            i += m.end()
        elif c == '%':
            m = re.match(r'%([01]+)', s[i:])
            yield ('n', int(m.group(1), 2))
            i += m.end()
        elif c.isdigit():
            m = re.match(r'\d+', s[i:])
            yield ('n', int(m.group(0)))
            i += m.end()
        elif c in '"\'':
            j = s.index(c, i + 1)
            v = 0
            for ch in s[i + 1:j]:
                v = v << 8 | ord(ch)
            yield ('n', v)
            i = j + 1
        elif re.match(r'[A-Za-z_.]', c):
            m = re.match(r'[A-Za-z_.][\w.]*', s[i:])
            yield ('s', m.group(0))
            i += m.end()
        elif s[i:i + 2] in ('<<', '>>'):
            yield ('o', s[i:i + 2])
            i += 2
        else:
            yield ('o', {'!': '|'}.get(c, c))
            i += 1

def value(expr, depth=0):
    """the value of a constant expression, or None"""
    out = []
    for t, v in tokens(expr):
        if t == 'n':
            out.append(str(v))
        elif t == 's':
            if v not in equ or depth > 40:
                return None
            x = value(equ[v], depth + 1)
            if x is None:
                return None
            out.append(f'({x})')
        else:
            out.append('//' if v == '/' else v)
    try:
        return int(eval(' '.join(out), {'__builtins__': {}}))
    except Exception:
        return None

def read_equates(path):
    rs = 0
    for line in open(path, encoding='latin-1'):
        label, op, operands = fields(strip_comment(line))
        lop = (op or '').lower()
        if lop == 'rsreset':
            rs = 0
        elif lop == 'rsset':
            rs = value(operands) or 0
        elif lop.startswith('rs.'):
            size = {'b': 1, 'w': 2, 'l': 4}[lop[3]]
            if size > 1:
                rs += rs & 1
            if label:
                equ[label] = str(rs)
            rs += size * (value(operands) or 0)
            equ['__RS'] = str(rs)
        elif label and lop in ('equ', 'set'):
            equ[label] = operands.replace('__RS', str(rs))
        elif label and op is None and '=' in line:
            m = re.match(r'^\s*[\w.]+\s*=\s*(\S+)', strip_comment(line))
            if m:
                equ[label] = m.group(1)

for path in args.equ:
    read_equates(path)

# ---- the statements of the sources -------------------------------------
COND = {'debug': 0, 'ezflag': 0}

def cond(expr):
    m = re.match(r'^(\w+)\s*=\s*(\d+)$', expr.strip())
    if m:
        return int(COND.get(m.group(1).lower(), 0) == int(m.group(2)))
    return COND.get(expr.strip().lower(), value(expr) or 0)

def find(name):
    for d in args.include:
        p = os.path.join(d, name)
        if os.path.exists(p):
            return p
    return None

inlined = set()         # sources included by others (+B.s: +Verif.s)
macros = {}             # name -> its lines (macros without parameters: Version)

def macro_definitions(path):
    """the macros of an include that is not read for its statements"""
    name = None
    for line in open(path, encoding='latin-1'):
        code = strip_comment(line)
        label, op, operands = fields(code) if code.strip() else (None, None, '')
        lop = (op or '').lower()
        if name is not None:
            if lop == 'endm':
                name = None
            else:
                macros[name].append(line)
        elif lop == 'macro' and label:
            name = label.lower()
            macros[name] = []

def statements(path, lines=None):
    """(labels, opcode, operands, where): conditionals, macro definitions,
    the RS layouts and the equates done; the jump lists included"""
    out = []
    labels = []
    stack = []
    active = True
    in_macro = False
    rs = 0
    for n, line in enumerate(lines if lines is not None else open(path, encoding='latin-1'), 1):
        code = strip_comment(line)
        if not code.strip():
            continue
        label, op, operands = fields(code)
        lop = (op or '').lower()
        if in_macro:
            if lop == 'endm':
                in_macro = False
            continue
        if lop in ('ifeq', 'ifne', 'ifd', 'ifnd', 'ifc', 'ifnc'):
            take = {'ifeq': lambda: cond(operands) == 0, 'ifne': lambda: cond(operands) != 0,
                    'ifd': lambda: operands.lower() in COND, 'ifnd': lambda: operands.lower() not in COND,
                    'ifc': lambda: True, 'ifnc': lambda: True}[lop]()
            stack.append(active)
            active = active and take
            continue
        if lop == 'else':
            active = stack[-1] and not active
            continue
        if lop == 'endc':
            active = stack.pop()
            continue
        if not active:
            continue
        if lop == 'macro':
            in_macro = True
            continue
        if lop == 'rsreset':
            rs = 0
            continue
        if lop.startswith('rs.'):
            size = {'b': 1, 'w': 2, 'l': 4}[lop[3]]
            if label:
                equ[label] = str(rs)
            rs += size * (value(operands) or 0)
            equ['__RS'] = str(rs)
            continue
        if label and lop in ('equ', 'set'):
            equ[label] = operands.replace('__RS', str(rs))
            continue
        if label and op is None and re.match(r'^\s*[\w.]+\s*=', code):
            equ[label] = code.split('=', 1)[1].strip().split()[0]
            continue
        if lop == 'include':
            if label:
                labels.append(label)
            f = operands.strip('"\'')
            if re.search(r'_Jumps\.s$', f) or f in args.sources:
                if f in args.sources:
                    inlined.add(f)
                p = find(f)
                sub = statements(p)
                if sub and labels:
                    sub[0] = (labels + sub[0][0],) + sub[0][1:]
                    labels = []
                out += sub
                continue
            if labels:
                out.append((labels, op, operands, f'{path}:{n}'))
                labels = []
            if p := find(f):
                macro_definitions(p)
            continue
        if label:
            labels.append(label)
        if op is None:
            continue
        if lop in macros and not operands:
            sub = statements(f'{path}:{n}', macros[lop])
            if sub and labels:
                sub[0] = (labels + sub[0][0],) + sub[0][1:]
                labels = []
            out += sub
            continue
        out.append((labels, op, operands, f'{path}:{n}'))
        labels = []
    if labels:
        out.append((labels, None, '', path))
    return out

# ---- the glue macros: the entries they make ---------------------------
# kind, operands
GLUE = {
    'LibCall': ('libcall', 'name,fn,params,result'),     # src/glue.i
    'LibNop': ('nop', 'name'),
    'RegRoutine': ('reg', 'name,fn'),
    'InCall': ('call', 'name,fn'),
    'RoutineCall': ('call', 'name,fn'),
    'TestCall': ('test', 'name,fn'),
    'FnCall': ('eval', 'name,fn'),
    'EvalRoutine': ('eval', 'name,fn'),
    'FnCallD3': ('eval_d3', 'name,fn'),
    'RoutineRecord': ('record', 'name,fn'),
    'CRoutine': ('c_routine', 'name,fn'),
    'FloatPar': ('float_', 'name,fn'),                    # src/lib/maths_glue.s
    'FloatEntry': ('float_', 'name,fn'),
    'WEntry': ('w', 'fn,size'),                          # src/w/wglue.i
    'WEntryA0': ('w_a0', 'fn'),
    'WEntryF': ('w_f', 'fn'),
    'WiEntry': ('window', 'n,size'),                     # src/w/windows_glue.s
    'StackEntry': ('stack', 'name,target'),              # src/glue.i
    'PrgEntry': ('w', 'prg'),                            # src/verif
    'VerEntry': ('w', 'ver'),
}
LABELLED = {'WEntry', 'WEntryA0', 'WEntryF', 'WiEntry'}     # their entry is the line's label
GETP = {'LibCall', 'LibNop', 'InCall', 'FnCall', 'FnCallD3', 'FloatPar'}
RESULT = {'Int': 0, 'Float': 1, 'String': 2, 'None': 3, 'Double': 4}
IGNORED = {'include', 'incdir', 'section', 'xref', 'opt', 'output', 'cnop', 'list', 'nolist', 'end', 'idnt',
           'machine', 'mc68000', 'mcinit', 'mc', 'rept', 'endr', 'fail', 'printt', 'printv', 'ttl'}
JUMPS = {'jmp': 6, 'bra': 4, 'bra.w': 4, 'bra.s': 2, 'bra.b': 2}

def cname(sym):
    return sym[1:] if sym.startswith('_') else sym

# ---- pass 1: the exported labels ----------------------------------------
parsed = {}
for path in args.sources:
    if path not in inlined:
        parsed[path] = statements(path)
exported = set()
for stmts in parsed.values():
    for labels, op, operands, where in stmts:
        if (op or '').lower() == 'xdef':
            exported.update(split_operands(operands))
        if op in ('LibRoutine', 'LibEntry'):
            exported.add(operands)
        if op in GLUE:
            ops = split_operands(operands)
            if 'name' in GLUE[op][1].split(','):
                exported.add(ops[0])
            if op == 'PrgEntry':
                exported.add(f'_ver_prg_{ops[0]}_entry')
            if op == 'VerEntry':
                exported.add(f'_ver_{ops[0]}_entry')
            if op in ('RegRoutine', 'RoutineCall', 'InCall', 'LibCall', 'FnCall', 'EvalRoutine', 'FnCallD3',
                      'FloatPar', 'FloatEntry', 'LibNop', 'TestCall', 'CRoutine', 'RoutineRecord'):
                exported.add(ops[0])

exported_lower = {e.lower(): e for e in exported}

# ---- pass 2: the layout ---------------------------------------------------
class Obj:
    def __init__(self, name):
        self.name = name
        self.bytes = bytearray()
        self.entries = []       # (offset, kind, fn, arg1, arg2, label, target)
        self.relocs = []        # (offset, size, expression, source)

    def emit(self, b):
        self.bytes += b

    def align(self, n=2):
        while len(self.bytes) % n:
            self.bytes.append(0)

objects = []
labels_at = {}          # key -> (obj, offset)
problems = []
functions = set()

def key(label, source):
    """labels are not case-sensitive (Devpac)"""
    l = label.lower()
    if l in exported_lower:
        return exported_lower[l]
    return f'{source}::{l}'

def place(label, obj, source, where):
    k = key(label, source)
    if k in labels_at:
        problems.append(f'{where}: {label} twice')
        return
    labels_at[k] = (obj, len(obj.bytes))

cxx = dict(c.split('=', 1) for c in args.cxx)

def symbol_ref(sym, source):
    """a symbol of an expression, as C++"""
    if sym in cxx:
        return f'uint32_t({cxx[sym]})'
    v = value(sym)
    if v is not None:
        return f'{v & 0xFFFFFFFF}u'
    k = key(sym, source)
    return ('L', k)

def cexpr(expr, source, local):
    out = []
    for t, v in tokens(expr):
        if t == 'n':
            out.append(f'{v & 0xFFFFFFFF}u')
        elif t == 's':
            if v.startswith('.'):
                v = local + v
            r = symbol_ref(v, source)
            out.append(r if isinstance(r, str) else r)
        else:
            out.append(v)
    return out

for path, stmts in parsed.items():
    obj = Obj(re.sub(r'\W', '_', path))
    objects.append(obj)
    local = ''
    code = False        # in a routine of assembler (after a hand entry)
    for labels, op, operands, where in stmts:
        lop = (op or '').lower()
        named = []
        for l in labels:
            if l.startswith('.'):
                l = local + l
            else:
                local = l
            named.append(l)
        if op in ('LibRoutine', 'LibEntry'):
            # the label of a routine of assembler (LibEntry: "GetP" before it)
            obj.align()
            if op == 'LibEntry':
                obj.emit(b'GetP')
            named.append(operands)
            local = operands
            hand = next((cname(l) for l in named if cname(l) in hand_impl), None)
            for l in named:
                place(l, obj, path, where)
            obj.entries.append((len(obj.bytes), 'hand' if hand else 'missing', hand, 0, 0, operands, None))
            obj.emit(bytes(4))
            code = True
            continue
        if lop == 'section':
            obj.align(4)            # (a section starts aligned)
        if lop == 'xdef' or lop in IGNORED and lop != 'include':
            for l in named:
                place(l, obj, path, where)
            continue
        if op in GLUE:
            kind, spec = GLUE[op]
            ops = dict(zip(spec.split(','), split_operands(operands)))
            entry = named[-1] if op in LABELLED and named else None
            for l in named:
                if l != entry:
                    place(l, obj, path, where)
            name = ops.get('name', entry)
            fn = ops.get('fn')
            if op == 'PrgEntry':
                name, fn = f'_ver_prg_{ops["prg"]}_entry', f'_ver_prg_{ops["prg"]}'
            if op == 'VerEntry':
                name, fn = f'_ver_{ops["ver"]}_entry', f'_ver_{ops["ver"]}'
            fn = cname(fn) if fn else None
            if fn:
                functions.add(fn)
            target = key(ops['target'], path) if kind == 'stack' else None
            a1 = a2 = 0
            if kind == 'libcall':
                a1, a2 = int(ops['params']), RESULT[ops['result']]
            if kind == 'window':
                a1 = int(ops['n'])
            if kind in ('w', 'window'):
                a2 = {'b': 1, 'w': 2, 'l': 4}.get(ops.get('size', 'l').lower(), 4)
            obj.align()
            if op == 'RoutineRecord':
                place(name, obj, path, where)
                obj.emit(bytes(4))
            elif op in GETP:
                if kind == 'libcall':
                    obj.relocs.append((len(obj.bytes), 4, [f'uint32_t(uintptr_t(&{fn}))'], where))
                    obj.emit(bytes(4) + bytes([a1 >> 8, a1 & 255, a2 >> 8, a2 & 255]) + b'LCal')
                obj.emit(b'GetP')
            if op != 'RoutineRecord':
                place(name, obj, path, where)
            obj.entries.append((len(obj.bytes), kind, fn, a1, a2, name, target))
            obj.emit(bytes(4))
            code = False
            continue
        if lop == 'include':
            for l in named:
                place(l, obj, path, where)
            img = images.get(cname(named[0])) if named else None
            if img:
                obj.emit(open(img, 'rb').read())
            elif named:
                problems.append(f'{where}: an Include under {named[0]}')
            continue
        if lop == 'incbin':
            for l in named:
                place(l, obj, path, where)
            p = find(operands.strip('"\''))
            if p:
                obj.emit(open(p, 'rb').read())
            else:
                problems.append(f'{where}: incbin {operands}')
            continue
        if lop.startswith('dc'):
            size = {'b': 1, 'w': 2, 'l': 4}[lop[3]] if '.' in lop else 2
            if size > 1:
                obj.align()
            for l in named:
                place(l, obj, path, where)
            for item in split_operands(operands):
                if not item:
                    continue
                if size == 1 and len(item) > 1 and item[0] in '"\'' and item[-1] == item[0]:
                    obj.emit(item[1:-1].encode('latin-1'))
                    continue
                v = value(item)
                if v is not None:
                    obj.emit((v & ((1 << 8 * size) - 1)).to_bytes(size, 'big'))
                    continue
                obj.relocs.append((len(obj.bytes), size, cexpr(item, path, local), where))
                obj.emit(bytes(size))
            code = False
            continue
        if lop.startswith('ds'):
            size = {'b': 1, 'w': 2, 'l': 4}[lop[3]] if '.' in lop else 2
            if size > 1:
                obj.align()
            for l in named:
                place(l, obj, path, where)
            v = value(operands)
            if v is None:
                problems.append(f'{where}: ds {operands}')
                v = 0
            obj.emit(bytes(v * size))
            code = False
            continue
        if lop == 'even':
            obj.align()
            for l in named:
                place(l, obj, path, where)
            continue
        if op is None:
            for l in named:
                place(l, obj, path, where)
            continue
        if lop in JUMPS and (named or not code):
            # an entry that goes on to its target (a table's or a label's)
            obj.align()
            for l in named:
                place(l, obj, path, where)
            target = operands
            if target.startswith('.'):
                target = local + target
            obj.entries.append((len(obj.bytes), 'alias', None, 0, 0, named[0] if named else target,
                                key(target, path)))
            obj.emit(bytes(JUMPS[lop]))
            code = False
            continue
        if lop == 'jjmp':
            obj.align()
            for l in named:
                place(l, obj, path, where)
            obj.entries.append((len(obj.bytes), 'lib_jump', None, value(operands) or 0, 0,
                                named[0] if named else operands, None))
            obj.emit(bytes(4))
            code = True
            continue
        # assembler: an entry the host does with C++ of its own
        if any(not l.startswith('.') for l in labels) or named and not code:
            obj.align()
            for l in named:
                place(l, obj, path, where)
            hand = next((cname(l) for l in named if cname(l) in hand_impl), None)
            obj.entries.append((len(obj.bytes), 'hand' if hand else 'missing', hand, 0, 0, named[0], None))
            obj.emit(bytes(4))
        elif named:
            for l in named:
                place(l, obj, path, where)
        code = True

# ---- output ---------------------------------------------------------------
def oid(o):
    return f'host_obj_{o.name}'

def addr(k):
    if k in labels_at:
        o, off = labels_at[k]
        return f'({oid(o)} + {off})'
    name = cname(k.split('::')[-1])
    functions.add(name)
    return f'(reinterpret_cast<const uint8_t *>(&{name}))'

def render(expr):
    out = []
    for t in expr:
        if isinstance(t, tuple):
            out.append(f'uint32_t(uintptr_t{addr(t[1])})')
        else:
            out.append(t)
    return '(' + ' '.join(out) + ')'

c_names = {cname(k) for k in labels_at if '::' not in k}
with open(args.out, 'w') as f:
    w = f.write
    w('// Generated by tools/host_glue.py: the assembler\'s labels, tables and glue\n')
    w('// entries on another host (src/host/glue.cpp)\n')
    w('#include "host/glue.h"\n\nusing namespace amos;\nusing namespace amos::host;\n\n')
    body = []
    count = 0
    body.append('namespace amos::host {\n\nconst GlueEntry glue_entries[] = {\n')
    for o in objects:
        for off, kind, fn, a1, a2, name, target in o.entries:
            fx = 'nullptr'
            if kind == 'hand':
                fx = f'reinterpret_cast<const void *>(&host_{fn})'
            elif fn:
                fx = f'reinterpret_cast<const void *>(&{fn})'
            tx = f'reinterpret_cast<const void *>{addr(target)}' if target else 'nullptr'
            body.append(f'    {{{oid(o)} + {off}, GlueKind::{kind}, {fx}, {tx}, {a1}, {a2}, "{cname(name)}"}},\n')
            count += 1
    body.append(f'}};\nconst int glue_entry_count = {count};\n\n')
    body.append('void glue_relocate()\n{\n')
    for o in objects:
        for off, size, expr, where in o.relocs:
            body.append(f'    put({oid(o)} + {off}, {size}, {render(expr)});      // {where}\n')
    body.append('}\n\n} // namespace amos::host\n')
    hands = sorted({e[2] for o in objects for e in o.entries if e[0] is not None and e[1] == 'hand'})
    w('extern "C" {\n')
    for fn in sorted(functions - c_names):
        w(f'void {fn}();\n')
    for o in objects:
        size = max(len(o.bytes), 4)
        init = ','.join(str(b) for b in o.bytes) if any(o.bytes) else ''
        w(f'alignas(4) uint8_t {oid(o)}[{size}] = {{{init}}};\n')
    w('}\n\nnamespace amos::host {\n')
    for h in hands:
        w(f'void host_{h}(AllRegs &r);\n')
    w('}\n\n')
    for k, (o, off) in sorted(labels_at.items()):
        if '::' not in k and re.match(r'^\w+$', cname(k)) and cname(k) not in defined:
            c = cname(k)
            w(f'__asm__(".globl {c}\\n.set {c}, {oid(o)}+{off}");\n')
    w('\n')
    w(''.join(body))

for p in problems:
    print('host_glue:', p, file=sys.stderr)
missing = sorted({cname(e[5]) for o in objects for e in o.entries if e[1] == 'missing'})
print(f'host_glue: {count} entries, {len(missing)} of assembler without C++', file=sys.stderr)
if args.list_missing:
    for m in missing:
        print('  missing', m, file=sys.stderr)
