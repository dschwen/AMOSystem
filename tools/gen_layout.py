#!/usr/bin/env python3
"""Generate src/core/layout.h from the RS layouts of the equate files.

The assembler describes its data structures with RS counters (+Equ.s,
+CEqu.s) and with the Rl/Rw/Rb macros of +WEqu.s, which count down from the
a5 data pointer. This tool collects every structure and lets vasm compute
the offsets and sizes (the field counts are assembler expressions), so the
C++ view of the data can never disagree with the assembler.

Every structure becomes a C++ struct with the assembler's field names, and
every field gets a static_assert on its offset. Structures:
- one per RsReset block (bobs, screens, menus, the interpreter data...)
- WData, the AMOS.library data below a5 (the T_ fields, without "T_")
- Window, which +Equ.s describes with a chain of equ's
It also lists the function numbers of the AMOS.library vectors (SyCall,
EcCall, WiCall) as amos::sy::X, amos::ec::X and amos::wi::X.

  gen_layout.py --out FILE [--vasm VASM] [vasm include options...]
"""

import argparse
import os
import re
import struct
import subprocess
import sys
import tempfile

FILES = ['+Equ.s', '+CEqu.s', '+WEqu.s']

# First field of a block -> struct name
NAMES = {
    'BbPrev': 'Bob',
    'EcLogic': 'Screen',
    'MnPrev': 'MenuItem',
    'Sl_Sx': 'Slider',
    'Bt_Number': 'Button',
    'PsCode': 'PacScreenHeader',
    'Pkcode': 'PacPictureHeader',
    'LEd_Buffer': 'LineEdit',
    'Dia_Channel': 'Dialog',
    'Dia_Ln': 'DiaZone',
    'Dia_BtRDraw': 'DiaButton',
    'Dia_LEd': 'DiaEdit',
    'Dia_LiTx': 'DiaList',
    'Dia_TxTx': 'DiaText',
    'Dia_Sl': 'DiaSlider',
    'Dia_KyCode': 'DiaKey',
    'Dia_BlNumber': 'DiaBlock',
    'VblRout': 'IData',
    'Prg_Next': 'Program',
    'Edt_Next': 'EditWindow',
    'Fs_Sp': 'FileSelector',
    'RnDY': 'Rainbow',
    'Short_Name': 'ShortSave',
    'APrg_MathFlags': 'CompiledProc',
}

# Element size of the equ-chain fields that are arrays
CHAIN_UNITS = {'WiColor': 4, 'WiColFl': 4, 'WiCuDraw': 2}

UNIT = {'b': 1, 'w': 2, 'l': 4}
CTYPE = {1: 'uint8_t', 2: 'uint16_t', 4: 'uint32_t'}

LABEL_RE = re.compile(r'^([A-Za-z_.][\w.]*):?$')


def split_line(line):
    """(label, opcode, operand) of a Devpac source line, comments removed."""
    line = line.rstrip('\n').replace('\r', '')
    if not line or line[0] in '*;':
        return None, None, None
    label = None
    if not line[0].isspace():
        label = line.split(None, 1)[0]
        line = line[len(label):]
        label = label.rstrip(':')
    fields = line.split(None, 2)
    if not fields or fields[0].startswith(';'):
        return label, None, None
    opcode = fields[0].lower()
    operand = fields[1] if len(fields) > 1 and not fields[1].startswith(';') else ''
    return label, opcode, operand


def c_name(name):
    return name.replace('.', '_')


class Block:
    def __init__(self, kind):
        self.kind = kind        # 'rs', 'w' (Rl/Rw/Rb) or 'chain'
        self.items = []         # (kind, name, unit, count_expr)

    def first_field(self):
        return next((n for k, n, _, _ in self.items if k == 'field'), None)


def parse(src_dir):
    blocks = []
    rs_block = None
    w_block = Block('w')
    chain = None
    in_macro = False
    for fname in FILES:
        with open(os.path.join(src_dir, fname), encoding='latin-1') as f:
            for line in f:
                label, op, operand = split_line(line)
                if op == 'macro':
                    in_macro = True
                    continue
                if in_macro:
                    in_macro = op != 'endm'
                    continue
                if op in ('rsreset', 'rsset'):
                    rs_block = Block('rs')
                    if op == 'rsset':
                        rs_block.items.append(('base', None, 1, operand))
                    blocks.append(rs_block)
                    continue
                m = re.fullmatch(r'rs\.([bwl])', op or '')
                if m and rs_block is not None:
                    kind = 'field' if label else 'pad'
                    rs_block.items.append((kind, label, UNIT[m.group(1)], operand))
                    continue
                if op in ('rl', 'rw', 'rb'):
                    name, count = operand.split(',', 1)
                    w_block.items.append(('field', 'T_' + name, UNIT[op[1]], count))
                    continue
                if op == 'equ' and label:
                    if operand.lower() == '__rs' and rs_block is not None:
                        rs_block.items.append(('alias', label, 1, None))
                        continue
                    # the window structure: WiPrev equ 0, WiNext equ WiPrev+4, ...
                    if label == 'WiPrev':
                        chain = Block('chain')
                        chain.name = 'Window'
                        blocks.append(chain)
                    if chain is not None:
                        base = operand.split('+')[0]
                        if label == 'WiPrev' or base in [n for _, n, _, _ in chain.items]:
                            chain.items.append(('field', label, None, None))
                            if label == 'WiLong':
                                chain = None
                            continue
                        chain = None
    w_block.name = 'WData'
    blocks.append(w_block)
    for b in blocks:
        if b.kind == 'rs':
            first = b.first_field()
            if first not in NAMES:
                sys.exit(f'gen_layout: no struct name for the block starting with {first}')
            b.name = NAMES[first]
    return [b for b in blocks if b.first_field()]


def evaluate(blocks, vasm, vasm_args):
    """Values of all names and count expressions, computed by vasm."""
    exprs = []
    for b in blocks:
        for kind, name, unit, count in b.items:
            if name:
                exprs.append(name)
            if count is not None:
                exprs.append(f'({count})')
    with tempfile.TemporaryDirectory() as tmp:
        src = os.path.join(tmp, 'probe.s')
        out = os.path.join(tmp, 'probe.bin')
        with open(src, 'w', encoding='latin-1') as f:
            f.write('\t\tInclude\t"+AMOS_Includes.s"\n')
            for e in exprs:
                f.write(f'\t\tdc.l\t{e}\n')
        subprocess.run([vasm, '-quiet', '-devpac', '-Fbin', '-o', out, *vasm_args, src], check=True, stdout=subprocess.DEVNULL)
        with open(out, 'rb') as f:
            data = f.read()
    values = struct.unpack(f'>{len(exprs)}l', data)
    return dict(zip(exprs, values))


def layout(block, values):
    """[(offset, size, unit, name)] fields, [(offset, name)] aliases, total size"""
    fields, aliases = [], []
    if block.kind == 'chain':
        names = [n for _, n, _, _ in block.items]
        offs = [values[n] for n in names]
        # the last name (WiLong) is the size
        for name, off, nxt in zip(names, offs, offs[1:]):
            size = nxt - off
            unit = CHAIN_UNITS.get(name, size if size in (2, 4) else 1)
            fields.append((off, size, unit, name))
        aliases.append((offs[-1], names[-1]))
        return fields, aliases, offs[-1]
    pos = 0
    pending_pad = False
    for kind, name, unit, count in block.items:
        if kind == 'base':
            pos = values[f'({count})']
            continue
        if kind == 'alias':
            aliases.append((values[name], name))
            if pending_pad:
                pos, pending_pad = values[name], False
            continue
        if kind == 'pad' and '__rs' in count.lower():
            # padding up to a fixed offset: vasm evaluated __RS at the end of
            # the probe, so the next name tells where it ends
            pending_pad = True
            continue
        if pending_pad:
            pos, pending_pad = values[name], False
        size = unit * values[f'({count})']
        start = values[name] if name else pos
        if block.kind == 'w':
            # counts down from a5: the field starts where the counter ends
            fields.append((start, size, unit, name))
            continue
        if kind == 'field':
            if start != pos:
                sys.exit(f'gen_layout: {name} at {start}, expected {pos}')
            fields.append((start, size, unit, name))
        else:
            fields.append((start, size, unit, None))
        pos = start + size
    if block.kind == 'w':
        fields.sort()
        low = fields[0][0]
        return fields, aliases, -low
    return fields, aliases, pos


def emit_struct(out, block, values):
    fields, aliases, total = layout(block, values)
    name = block.name
    base = 0
    if block.kind == 'w':
        base = fields[0][0]
        out.append(f'// AMOS.library data below a5 ({total} bytes): field T_X is at a5+T_X.')
    elif block.kind == 'rs' and block.items[0][0] == 'base':
        base = fields[0][0]
    out.append(f'struct {name} {{')
    pad = 0
    pos = base
    for off, size, unit, fname in fields:
        if off > pos:
            out.append(f'    uint8_t _pad{pad}[{off - pos}];')
            pad += 1
        elif off < pos:
            sys.exit(f'gen_layout: {name}.{fname} overlaps the previous field')
        if fname is None:
            out.append(f'    uint8_t _pad{pad}[{size}];')
            pad += 1
        else:
            member = c_name(fname[2:] if block.kind == 'w' else fname)
            n = size // unit
            dim = f'[{n}]' if n != 1 or size % unit else ''
            if size % unit:
                dim = f'[{size}]'
                unit = 1
            if size == 0:
                out.append(f'    // {member}: empty')
                continue
            out.append(f'    {CTYPE[unit]} {member}{dim};')
        pos = off + size
    end = base + total if block.kind != 'w' else 0
    if pos < end:
        out.append(f'    uint8_t _pad{pad}[{end - pos}];')
    for off, aname in aliases:
        out.append(f'    static constexpr int32_t {c_name(aname)} = {off};')
    out.append('};')
    out.append(f'static_assert(sizeof({name}) == {total});')
    for off, size, unit, fname in fields:
        if fname and size:
            member = c_name(fname[2:] if block.kind == 'w' else fname)
            out.append(f'static_assert(offsetof({name}, {member}) == {off - base});')
    out.append('')


# AMOS.library vector functions: namespace, first name, the macro after them
VECTORS = [('sy', 'Inkey', 'SyCall'), ('ec', 'Raz', 'EcCall'), ('wi', 'ChrOut', 'WiCall')]


def vector_functions(src_dir):
    """{namespace: [(name, number)]} from the equ lists in +Equ.s"""
    result = {}
    with open(os.path.join(src_dir, '+Equ.s'), encoding='latin-1') as f:
        lines = [split_line(line) for line in f]
    for ns, first, end in VECTORS:
        names = []
        active = False
        for label, op, operand in lines:
            active = active or label == first
            if not active:
                continue
            if label == end:
                break
            if label and op == 'equ':
                names.append((label, int(operand)))
        result[ns] = names
    return result


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--out', required=True)
    p.add_argument('--src', default='.')
    p.add_argument('--vasm', default='vasmm68k_mot')
    args, vasm_args = p.parse_known_args()
    blocks = parse(args.src)
    values = evaluate(blocks, args.vasm, vasm_args)
    out = [
        '// Generated by tools/gen_layout.py from +Equ.s, +CEqu.s and +WEqu.s.',
        '// Do not edit: change the assembler definitions instead.',
        '#pragma once',
        '',
        '#include <stddef.h>',
        '#include <stdint.h>',
        '',
        'namespace amos {',
        '',
    ]
    for b in blocks:
        emit_struct(out, b, values)
    for ns, names in vector_functions(args.src).items():
        out.append(f'namespace {ns} {{')
        out.extend(f'constexpr int {name} = {num};' for name, num in names)
        out.append(f'}} // namespace {ns}')
        out.append('')
    out.append('} // namespace amos')
    with open(args.out, 'w') as f:
        f.write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()
