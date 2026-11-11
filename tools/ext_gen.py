#!/usr/bin/env python3
"""The third-party extensions' tables (docs/THIRD-PARTY-EXTENSIONS.md): Turbo Plus and AMCAF.

For each extension, from its original library file (AMOS/APSystem/AMOSPro_*.Lib: only its token
table and title are read, none of its code) and its manifest (data/extensions/NAME.json: what
each command is called in C++, and whether it is implemented), this writes

  src/tables/ext_NAME_image.cpp   the library image: the token table (AP20 form, offsets as in the
                                  original), the routines' sizes (all 0: they are C++), the title
  src/tables/ext_NAME_glue.cpp    a glue entry for each command's routine and the cold start, and
                                  the list that binds them to the library's jump table
  src/ext/NAME_stubs.cpp          the commands not implemented yet (they raise an AMOS error)
  src/tables/generated/third_party.h, .../third_party_entries.inc, .../third_party_relocate.inc,
  and the labels of src/tables/labels.cpp/.h, tables.h, index.cpp between their markers

and the owners of src/runtime/bindings/owners.json. Then run tools/sync_static_segments.py and
tools/sync_runtime_bindings.py (this does, unless --no-sync).

  ext_gen.py --init NAME   (re)make the manifest from the library, keeping what it says already
  ext_gen.py               generate
  ext_gen.py --check       verify that what is committed is what this would write
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.dont_write_bytecode = True

EXTENSIONS = {
    'amcaf': dict(file='AMOSPro_AMCAF.Lib', slot=8, name='AMCAF', prefix='amcaf'),
    'turbo': dict(file='AMOSPro_TURBO_Plus.Lib', slot=12, name='Turbo Plus', prefix='turbo'),
}
MARK = '// third-party extensions (tools/ext_gen.py): '
RESULT = {'I': 3, '0': 0, '2': 2}                      # the glue's result kind: none, integer, string


# ---- the original library ---------------------------------------------------------------------

class Token:
    def __init__(self, offset, instruction, function, name, signature, terminator):
        self.offset, self.instruction, self.function = offset, instruction, function
        self.raw_name, self.signature, self.terminator = name, signature, terminator


def read_library(path):
    b = Path(path).read_bytes()
    if struct.unpack_from('>I', b, 0)[0] != 0x3F3 or struct.unpack_from('>I', b, 24)[0] != 0x3E9:
        raise ValueError(f'{path}: not a single-hunk AMOS library')
    sizes_size, tokens_size, code_size, title_size = struct.unpack_from('>4I', b, 32)
    at = 50
    if b[at:at + 4] == b'AP20':
        raise ValueError(f'{path}: already an AP20 library (this reads the old form)')
    tokens_at = at + sizes_size
    title_at = tokens_at + tokens_size + code_size
    table = b[tokens_at:tokens_at + tokens_size]
    prefix = table[:6]
    tokens, p = [], 6
    while struct.unpack_from('>H', table, p)[0]:
        start = p
        instruction, function = struct.unpack_from('>HH', table, p)
        p += 4
        name_at = p
        while table[p] < 128:
            p += 1
        p += 1
        name = table[name_at:p]
        sig_at = p
        while table[p] < 128:
            p += 1
        signature = table[sig_at:p]
        terminator = table[p]
        p += 1
        p += p & 1
        tokens.append(Token(start, instruction, function, name, signature, terminator))
    assert table[p:p + 2] == b'\0\0' and p + 2 == len(table), 'token table end'
    return dict(sha256=hashlib.sha256(b).hexdigest(), routines=sizes_size // 2, prefix=prefix, tokens=tokens,
                table_size=tokens_size, title=b[title_at:title_at + title_size], bytes=len(b))


def command_name(token, previous):
    name = ''.join(chr(c & 127) for c in token.raw_name)
    if name.startswith('!'):
        name = name[1:]
    elif name == '\x00':                 # (the 0x80 record: the name of the one before)
        name = previous
    return name


def slug(name):
    s = name.lower().replace('$', '_s')
    s = re.sub(r'[^a-z0-9_]+', '_', s).strip('_')
    return s


# ---- the manifest ----------------------------------------------------------------------------

def manifest_path(key):
    return ROOT / 'data/extensions' / (key + '.json')


def init_manifest(key, library):
    info = EXTENSIONS[key]
    old = {}
    if manifest_path(key).exists():
        old = {c['offset']: c for c in json.loads(manifest_path(key).read_text())['commands']}
    commands, names, previous = [], {}, ''
    for t in library['tokens']:
        name = command_name(t, previous)
        previous = name
        sig = t.signature.decode('ascii')
        routine = t.instruction if t.instruction != 0xFFFF else t.function
        base = slug(name)
        if not base.startswith(info['prefix'] + '_'):
            base = f"{info['prefix']}_{base}"
        names.setdefault(base, []).append(t.offset)
        commands.append(dict(offset=t.offset, name=name, signature=sig, routine=routine,
                             terminator=t.terminator, symbol=base, status='stub', source=''))
    for base, offsets in names.items():
        if len(offsets) > 1:
            by_count = {}
            for c in commands:
                if c['offset'] in offsets:
                    by_count.setdefault(len(params_of(c)), []).append(c)
            for n, group in by_count.items():
                for i, c in enumerate(group):
                    c['symbol'] = f'{base}_{n}' + ('' if len(group) == 1 else 'abcdefgh'[i])
    first = {}
    for c in commands:
        if c['routine'] in first:
            c['symbol'] = first[c['routine']]['symbol']          # (two tokens, one routine)
        first.setdefault(c['routine'], c)
    for c in commands:
        if c['offset'] in old:
            c['symbol'] = old[c['offset']]['symbol']
            c['status'] = old[c['offset']]['status']
            c['source'] = old[c['offset']]['source']
    data = dict(extension=info['name'], key=key, file=info['file'], defaultSlot=info['slot'],
                libSha256=library['sha256'], routines=library['routines'], commands=commands)
    manifest_path(key).parent.mkdir(parents=True, exist_ok=True)
    manifest_path(key).write_text(json.dumps(data, indent=1) + '\n')
    print(f'{key}: {len(commands)} commands, {sum(c["status"] != "stub" for c in commands)} implemented')


def kind_of(c):
    s = c['signature']
    return 'statement' if s[:1] == 'I' else 'function'


def params_of(c):
    """The parameter types of a command: '0' integer, '2' string."""
    s = c['signature']
    out, i = [], 1
    while i < len(s):
        ch = s[i]
        if ch in '02':
            out.append(ch)
        i += 1
    return out


def result_kind(c):
    return 3 if kind_of(c) == 'statement' else RESULT[c['signature'][0]]


def c_params(c, names=False):
    types = {'0': 'int32_t', '2': 'GuestString'}
    return ', '.join(types[p] + (f' a{i}' if names else '') for i, p in enumerate(params_of(c)))


def c_result(c):
    return {3: 'void', 0: 'int32_t', 2: 'uint32_t'}[result_kind(c)]


# ---- the files -------------------------------------------------------------------------------

def be16(v):
    return struct.pack('>H', v)


def image_bytes(library):
    """Header, AP20 marker, routine sizes (0), the token table in AP20 form, the title."""
    table = bytearray(library['prefix'])
    for t in library['tokens']:
        # (absent is 0xFFFF in the old form, 1 in AP20)
        i = 1 if t.instruction == 0xFFFF else t.instruction
        f = 1 if t.function == 0xFFFF else t.function
        if i > 1 and f > 1:
            raise ValueError('a token with an instruction and a function')
        table += be16(i) + be16(f) + t.raw_name + t.signature + bytes([t.terminator])
        if len(table) & 1:
            table += b'\0'
    table += b'\0\0'
    assert len(table) == library['table_size'], (len(table), library['table_size'])
    sizes = library['routines'] * 2
    head = struct.pack('>4I', sizes, len(table), 0, len(library['title'])) + b'\0\0'
    return head + b'AP20' + bytes(sizes) + bytes(table) + library['title']


def cpp_array(name, data, comment):
    lines = []
    for i in range(0, len(data), 24):
        lines.append('    ' + ','.join(str(v) for v in data[i:i + 24]))
    return f'extern "C" {{\nalignas(4) uint8_t {name}[{len(data)}] = {{\n' + ',\n'.join(lines) + '};\n}\n'


def generate_image(key, library):
    seg = f'host_obj_src_ext_{key}_image_s'
    data = image_bytes(library)
    return (f'// {seg}: the token table and title of {EXTENSIONS[key]["file"]}, written by tools/ext_gen.py\n'
            '// (the library\'s code is C++, src/ext). Do not edit.\n'
            '#include "tables/tables.h"\n\nusing namespace amos;\nusing namespace amos::host;\n\n'
            + cpp_array(seg, data, '') +
            f'\nnamespace amos::host {{\n\nvoid relocate_src_ext_{key}_image_s()\n{{\n}}\n\n'
            f'const GlueEntry entries_src_ext_{key}_image_s[] = {{\n'
            '    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},\n};\n\n} // namespace amos::host\n'), len(data)


def bindings(manifest):
    """The routines that get a glue block: (routine, command) in the order of the routine numbers."""
    seen, out = {}, []
    for c in manifest['commands']:
        if c['routine'] in seen:
            # (two tokens, one routine: Wsave and Dsave; one binding)
            first = seen[c['routine']]
            if (params_of(c), result_kind(c)) != (params_of(first), result_kind(first)):
                raise ValueError(f'routine {c["routine"]}: two commands, two signatures')
            c['symbol'] = first['symbol']
            c['status'], c['source'] = first['status'], first['source']
            continue
        seen[c['routine']] = c
        out.append(c)
    out.sort(key=lambda c: c['routine'])
    return out


def pascal(symbol):
    return ''.join(w.capitalize() for w in symbol.split('_'))


def generate_glue(key, manifest):
    seg = f'host_obj_src_ext_{key}_glue_s'
    cmds = bindings(manifest)
    blocks = bytearray()
    for c in cmds:
        blocks += bytes(4) + be16(len(params_of(c))) + be16(result_kind(c)) + b'LCalGetP' + bytes(4)
    cold_at = len(blocks)
    blocks += bytes(4)                     # the cold start's entry
    hooks = []                             # the entries AMOS calls (ExtAdr): default, end...
    for name in manifest.get('hooks', []):
        hooks.append((name, len(blocks)))
        blocks += bytes(4)
    # the list that binds the routines to the jump table: its length, the cold start first,
    # then (routine, address)
    jumps_at = len(blocks)
    entries = [(0, cold_at)] + [(c['routine'], 20 * i + 16) for i, c in enumerate(cmds)]
    blocks += be16(manifest['routines'])
    list_at = len(blocks)
    for routine, at in entries:
        blocks += be16(routine) + bytes(4)
    blocks += be16(0)
    reloc = []
    for i, c in enumerate(cmds):
        reloc.append(f'    AMOS_LIBRARY_METADATA({seg} + {20 * i}, {c["symbol"]});')
    for n, (routine, at) in enumerate(entries):
        reloc.append(f'    put({seg} + {list_at + 6 * n + 2}, 4, AMOS_STATIC_ADDRESS({seg}, {at}));')
    rows = []
    for i, c in enumerate(cmds):
        rows.append(f'    {{AMOS_ENTRY_AT({seg}, {20 * i + 16}), GlueKind::libcall, AMOS_BIND_LIBRARY({c["symbol"]}), '
                    f'nullptr, {len(params_of(c))}, {result_kind(c)}, "Ext_{pascal(key)}_{pascal(c["symbol"])}"}},')
    rows.append(f'    {{AMOS_ENTRY_AT({seg}, {cold_at}), GlueKind::reg, AMOS_BIND_REG({key}_cold), nullptr, 0, 0, '
                f'"Ext_{pascal(key)}_Cold"}},')
    for name, at in hooks:
        rows.append(f'    {{AMOS_ENTRY_AT({seg}, {at}), GlueKind::call, AMOS_BIND_CALL({key}_{name}), nullptr, 0, 0, '
                    f'"{key}_{name}_entry"}},')
    text = (f'// {seg}: the glue of the commands of {EXTENSIONS[key]["file"]}, written by tools/ext_gen.py. Do not edit.\n'
            '#include "tables/tables.h"\n\nusing namespace amos;\nusing namespace amos::host;\n\n'
            + cpp_array(seg, bytes(blocks), '') +
            f'\nnamespace amos::host {{\n\nvoid relocate_src_ext_{key}_glue_s()\n{{\n' + '\n'.join(reloc) +
            f'\n}}\n\nconst GlueEntry entries_src_ext_{key}_glue_s[] = {{\n' + '\n'.join(rows) +
            '\n    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},\n};\n\n} // namespace amos::host\n')
    return text, len(blocks), jumps_at, cmds, hooks


def generate_stubs(key, manifest, cmds):
    info = EXTENSIONS[key]
    out = [f'// The commands of {info["name"]} that are not implemented yet: written by tools/ext_gen.py from\n'
           f'// data/extensions/{key}.json (a command leaves this file when its "status" there says "implemented"\n'
           '// and its "source" has it). Each raises an AMOS error.\n',
           '#include "core/amos.h"', '#include "ext/third_party.h"', '#include "lib/strings.h"\n', 'using namespace amos;\n',
           'extern "C" {\n']
    for c in cmds:
        if c['status'] != 'stub':
            continue
        out.append(f'// {c["name"]} ({c["signature"]})')
        out.append(f'{c_result(c)} {c["symbol"]}({c_params(c)}) {{ ext3::unsupported(ext3::Id::{key}); }}')
    out.append('\n}\n')
    out.append('#if !defined(__mc68000__)\n#include "runtime/bindings/ext_%s_stubs.inc"\n#endif' % key)
    return '\n'.join(out) + '\n'


def splice(text, tag, body):
    begin, end = f'{MARK}begin {tag}\n', f'{MARK}end {tag}\n'
    i, j = text.find(begin), text.find(end)
    if i < 0 or j < i:
        raise ValueError(f'markers for {tag} missing')
    return text[:i + len(begin)] + body + text[j:]


def generate(check=False, sync=True):
    outputs, owners = {}, {}
    declarations, functions, entries, relocate, labels_cpp, labels_h, m68k = [], [], [], [], [], [], []
    for key, info in EXTENSIONS.items():
        manifest = json.loads(manifest_path(key).read_text())
        lib = ROOT / 'AMOS/APSystem' / info['file']
        library = None
        image_path = ROOT / f'src/tables/ext_{key}_image.cpp'
        if lib.exists():
            library = read_library(lib)
            if library['sha256'] != manifest['libSha256']:
                raise ValueError(f'{lib}: not the library the manifest was made from')
            image, image_size = generate_image(key, library)
            outputs[image_path] = image
        elif image_path.exists():
            image_size = int(re.search(r'\[(\d+)\]', image_path.read_text())[1])
        else:
            sys.exit(f'{lib} is needed (the first time)')
        glue, glue_size, jumps_at, cmds, hooks = generate_glue(key, manifest)
        outputs[ROOT / f'src/tables/ext_{key}_glue.cpp'] = glue
        outputs[ROOT / f'src/ext/{key}_stubs.cpp'] = generate_stubs(key, manifest, cmds)
        stub_file = f'src/ext/{key}_stubs.cpp'
        for c in cmds:
            owners[c['symbol']] = stub_file if c['status'] == 'stub' else c['source']
            if c['status'] != 'stub' and not c['source']:
                raise ValueError(f'{c["symbol"]}: implemented but no source')
        owners[f'{key}_cold'] = f'src/ext/{key}.cpp'
        for name, at in hooks:
            owners[f'{key}_{name}'] = manifest.get('hookSource', f'src/ext/{key}.cpp')
        seg_g, seg_i = f'host_obj_src_ext_{key}_glue_s', f'host_obj_src_ext_{key}_image_s'
        declarations += [f'extern uint8_t {seg_g}[{glue_size}];', f'extern uint8_t {seg_i}[{image_size}];']
        functions += [f'void relocate_src_ext_{key}_glue_s();', f'extern const GlueEntry entries_src_ext_{key}_glue_s[];',
                      f'void relocate_src_ext_{key}_image_s();', f'extern const GlueEntry entries_src_ext_{key}_image_s[];']
        m68k += [f'void {c["symbol"]}();' for c in cmds] + [f'void {key}_cold();'] + [f'void {key}_{n}();' for n, _ in hooks]
        entries += [f'    entries_src_ext_{key}_glue_s,', f'    entries_src_ext_{key}_image_s,']
        relocate += [f'    relocate_src_ext_{key}_glue_s();', f'    relocate_src_ext_{key}_image_s();']
        labels_cpp += [f'uint8_t *const ext_{key}_image = {seg_i} + 0;',
                       f'uint8_t *const ext_{key}_jumps = {seg_g} + {jumps_at};']
        labels_h += [f'extern uint8_t *const ext_{key}_image;', f'extern uint8_t *const ext_{key}_jumps;']
        for name, at in hooks:
            labels_cpp.append(f'uint8_t *const ext_{key}_{name}_entry = {seg_g} + {at};')
            labels_h.append(f'extern uint8_t *const ext_{key}_{name}_entry;')
    outputs[ROOT / 'src/tables/generated/third_party.h'] = (
        '// Written by tools/ext_gen.py: the third-party extensions\' tables. (On the Amiga only with EXTRAS=1:\n'
        '// the Amiga loads the originals from their files.)\n#pragma once\n'
        '#if !defined(__mc68000__) || defined(AMOS_THIRD_PARTY)\n'
        '#include "runtime/glue.h"\n\nextern "C" {\n' + '\n'.join(declarations) + '\n}\n'
        'namespace amos::host {\n' + '\n'.join(functions) + '\n}\n'
        'extern "C" {\n#if defined(__mc68000__)\n' + '\n'.join(m68k) + '\n#endif\n}\n#endif\n')
    guard = '#if !defined(__mc68000__) || defined(AMOS_THIRD_PARTY)\n'
    outputs[ROOT / 'src/tables/generated/third_party_entries.inc'] = guard + '\n'.join(entries) + '\n#endif\n'
    outputs[ROOT / 'src/tables/generated/third_party_relocate.inc'] = guard + '\n'.join(relocate) + '\n#endif\n'
    for path, tag, lines in ((ROOT / 'src/tables/labels.cpp', 'labels', labels_cpp),
                             (ROOT / 'src/tables/labels.h', 'labels', labels_h)):
        outputs[path] = splice(outputs.get(path, path.read_text()), tag, guard + '\n'.join(lines) + '\n#endif\n')
    # (the owners of the bindings: the other ones stay)
    owners_path = ROOT / 'src/runtime/bindings/owners.json'
    current = json.loads(owners_path.read_text())
    for key in EXTENSIONS:
        for name in [n for n, f in current.items() if f.startswith(f'src/ext/{key}_stubs.cpp') or
                     (f.startswith(f'src/ext/{key}') and n.startswith(EXTENSIONS[key]['prefix'] + '_'))]:
            del current[name]
    current.update(owners)
    outputs[owners_path] = json.dumps(current, indent=2, sort_keys=True) + '\n'
    errors = []
    for path, text in outputs.items():
        if check:
            if not path.exists() or path.read_text() != text:
                errors.append(str(path.relative_to(ROOT)))
        elif not path.exists() or path.read_text() != text:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
    if errors:
        sys.exit('stale: ' + ', '.join(errors))
    if sync and not check:
        for tool in ('sync_static_segments.py', 'sync_runtime_bindings.py'):
            subprocess.check_call([sys.executable, str(ROOT / 'tools' / tool)])
    print('third-party extension tables ' + ('verified' if check else 'written'))


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--init', choices=EXTENSIONS, nargs='+', help='make the manifest from the library')
    ap.add_argument('--check', action='store_true')
    ap.add_argument('--no-sync', action='store_true')
    ap.add_argument('--lib', type=Path, help='the library file for --init (default AMOS/APSystem)')
    ap.add_argument('--mark', nargs='+', metavar=('KEY', 'SOURCE'),
                    help='KEY SOURCE SYMBOL...: these commands are implemented in SOURCE')
    ap.add_argument('--hooks', nargs='+', metavar=('KEY', 'HOOK'), help='KEY HOOK...: the entries AMOS calls (default, end)')
    args = ap.parse_args()
    if args.mark:
        key, source, *symbols = args.mark
        m = json.loads(manifest_path(key).read_text())
        found = set()
        for c in m['commands']:
            if c['symbol'] in symbols:
                c['status'], c['source'] = 'implemented', source
                found.add(c['symbol'])
        if found != set(symbols):
            sys.exit('not commands of ' + key + ': ' + ', '.join(sorted(set(symbols) - found)))
        manifest_path(key).write_text(json.dumps(m, indent=1) + '\n')
        generate(False, not args.no_sync)
    elif args.hooks:
        key, *names = args.hooks
        m = json.loads(manifest_path(key).read_text())
        m['hooks'] = names
        manifest_path(key).write_text(json.dumps(m, indent=1) + '\n')
        generate(False, not args.no_sync)
    elif args.init:
        for key in args.init:
            init_manifest(key, read_library(args.lib or ROOT / 'AMOS/APSystem' / EXTENSIONS[key]['file']))
    else:
        generate(args.check, not args.no_sync)
