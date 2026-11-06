#!/usr/bin/env python3
"""Describe static guest segments/relocations and replace hosted pointer casts.
The generated catalog retains native source bytes solely as explicit HostSpan
inputs. Relocation targets use segment identities and byte offsets.
"""
import argparse
from pathlib import Path
import re
ROOT=Path(__file__).resolve().parents[1]
OBJECT=re.compile(r'uint8_t\s+(host_obj_\w+)\[(\d+)\]\s*=')
PUT=re.compile(r'^(\s*)put\((host_obj_\w+) \+ (\d+), (\d+), (.*)\);(.*)$')
REF=re.compile(r'uint32_t\(uintptr_t\((host_obj_\w+) \+ (\d+)\)\)')
ABS=re.compile(r'AMOS_STATIC_ADDRESS\((host_obj_\w+), (\d+)\)')
DIFF=re.compile(r'AMOS_STATIC_DIFFERENCE\((host_obj_\w+), (\d+), (host_obj_\w+), (\d+)\)')
BRIDGE=re.compile(r'^(\s*)AMOS_BRIDGE_METADATA\((host_obj_\w+) \+ (\d+)\);(.*)$')

def generate(check=False):
    files=sorted((ROOT/'src/tables').glob('*.cpp'))
    segments={}; outputs={}; relocations=[]
    for p in files:
        text=p.read_text()
        for m in OBJECT.finditer(text):
            prefix=text[text.rfind('\n',0,m.start())+1:m.start()]
            if m[1] in segments: raise ValueError('duplicate segment '+m[1])
            segments[m[1]]=(int(m[2]), 'AMOS_CHIP' in prefix)
    if len(segments)>127: raise ValueError('segment binding capacity exceeded')
    for p in files:
        lines=[]
        for line in p.read_text().splitlines():
            line=re.sub(r'\{(host_obj_\w+) \+ (\d+), GlueKind::', r'{AMOS_ENTRY_AT(\1, \2), GlueKind::', line)
            if 'GlueKind::' in line:
                line=re.sub(r'reinterpret_cast<const void \*>\((host_obj_\w+) \+ (\d+)\)', r'AMOS_ENTRY_AT(\1, \2)', line)
            line=line.replace('{nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr}', '{AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr}')
            library=re.match(r'^\s*AMOS_LIBRARY_METADATA\((host_obj_\w+) \+ (\d+), \w+\);',line)
            if library:
                relocations.append((library[1],int(library[2]),4,'scalar','',0,'',0,'0'))
                lines.append(line);continue
            bridge=BRIDGE.match(line)
            if bridge:
                indent,source,offset,tail=bridge.groups()
                relocations.append((source,int(offset),4,'scalar','',0,'',0,'0'))
                lines.append(line);continue
            m=PUT.match(line)
            if not m:
                lines.append(line);continue
            indent,source,offset,width,expr,tail=m.groups(); offset=int(offset);width=int(width)
            refs=REF.findall(expr); absolute=ABS.search(expr); difference=DIFF.search(expr)
            if 'amos_bridge' in expr:
                if expr != '(uint32_t(uintptr_t(reinterpret_cast<const uint8_t *>(&amos_bridge))))':
                    raise ValueError('unclassified bridge expression: '+line)
                expr_kind='scalar'; target=''; to=0; anchor=''; ao=0; scalar='0'
                line=f'{indent}AMOS_BRIDGE_METADATA({source} + {offset});{tail}'
            elif refs or absolute or difference:
                scalar='0'
                if len(refs)==2:
                    if re.sub(r'\s+', '', REF.sub('S',expr)) != '(S-S)':
                        raise ValueError('unclassified relative expression: '+line)
                    target,to=refs[0];anchor,ao=refs[1];expr_kind='difference'
                elif len(refs)==1:
                    if re.sub(r'\s+', '', REF.sub('S',expr)) != '(S)':
                        raise ValueError('unclassified absolute expression: '+line)
                    target,to=refs[0];anchor='';ao=0;expr_kind='absolute'
                elif difference:
                    if re.sub(r'\s+', '', DIFF.sub('S',expr)) not in ('S','(S)'):
                        raise ValueError('unclassified relative expression: '+line)
                    target,to,anchor,ao=difference.groups();expr_kind='difference'
                elif absolute:
                    if re.sub(r'\s+', '', ABS.sub('S',expr)) not in ('S','(S)'):
                        raise ValueError('unclassified absolute expression: '+line)
                    target,to=absolute.groups();anchor='';ao=0;expr_kind='absolute'
                else:raise ValueError('unrecognized relocation: '+line)
                to=int(to);ao=int(ao)
                if expr_kind=='absolute':replacement=f'AMOS_STATIC_ADDRESS({target}, {to})'
                else:replacement=f'AMOS_STATIC_DIFFERENCE({target}, {to}, {anchor}, {ao})'
                line=f'{indent}put({source} + {offset}, {width}, {replacement});{tail}'
            else:
                if expr != '(uint32_t(sizeof(amos::WData)))':raise ValueError('unclassified scalar expression: '+line)
                expr_kind='scalar';target='';to=0;anchor='';ao=0;scalar=expr
            relocations.append((source,offset,width,expr_kind,target,to,anchor,ao,scalar))
            lines.append(line)
        rewritten='\n'.join(lines)+'\n'
        if rewritten!=p.read_text():outputs[p]=rewritten
    for source,offset,width,kind,target,to,anchor,ao,scalar in relocations:
        if width not in (2,4) or offset+width>segments[source][0]:raise ValueError('relocation outside source')
        for name,at in ((target,to),(anchor,ao)):
            if name and (name not in segments or not 0<=at<=segments[name][0]):raise ValueError('symbol outside target '+name)
    ids=['// Generated by tools/sync_static_segments.py.', '#pragma once','#include <stdint.h>',
         'namespace amos::tables {','enum class Segment : uint16_t {']
    ids += [f'    {name} = {i},' for i,name in enumerate(sorted(segments),1)]
    ids += ['};','}']
    outputs[ROOT/'src/tables/segment_ids.h']='\n'.join(ids)+'\n'
    data=['// Generated by tools/sync_static_segments.py. Included inside the loader namespace.',
          'const Source sources[] = {']
    for name,(size,chip) in sorted(segments.items()):
        data.append(f'    {{tables::Segment::{name}, guest::HostSpan<guest::Byte>({name}, {size}), {str(chip).lower()}}},')
    data += ['};','const Relocation relocations[] = {']
    for source,offset,width,kind,target,to,anchor,ao,scalar in relocations:
        t=f'tables::Segment::{target}' if target else 'tables::Segment{}'
        a=f'tables::Segment::{anchor}' if anchor else 'tables::Segment{}'
        data.append(f'    {{tables::Segment::{source}, {offset}, {width}, Kind::{kind}, {t}, {to}, {a}, {ao}, {scalar}}},')
    data += ['};']
    outputs[ROOT/'src/tables/generated/static_segments.inc']='\n'.join(data)+'\n'
    labels=re.findall(r'^uint8_t \*const (\w+) = (host_obj_\w+) \+ (\d+);$',
                      (ROOT/'src/tables/labels.cpp').read_text(),re.M)
    declarations=set(re.findall(r'extern uint8_t \*const (\w+);',(ROOT/'src/tables/labels.h').read_text()))
    if {name for name,_,_ in labels} != declarations:raise ValueError('unclassified exported static label')
    entries=set()
    for p in files:
        entries.update((segment,int(offset)) for segment,offset in
                       re.findall(r'\{AMOS_ENTRY_AT\((host_obj_\w+), (\d+)\), GlueKind::',outputs.get(p,p.read_text())))
    for p in files:
        for segment,offset in re.findall(r'AMOS_ENTRY_AT\((host_obj_\w+), (\d+)\)', outputs.get(p,p.read_text())):
            if segment not in segments or int(offset)+4 > segments[segment][0]:
                raise ValueError('code entry outside segment: '+segment+' + '+offset)
    symbols=['// Generated by tools/sync_static_segments.py.', '#pragma once',
             '#include "core/guest_symbol.h"', '#include "tables/segment_ids.h"',
             'namespace amos { struct AllRegs; }',
             'namespace amos::tables::data_symbols {']
    for name,segment,offset in labels:
        if int(offset)>segments[segment][0]:raise ValueError('label outside segment: '+name)
        symbols.append(f'inline constexpr guest::GuestSymbol<guest::Byte> {name}(guest::SegmentId(uint16_t(Segment::{segment})), {offset});')
    symbols += ['}', 'namespace amos::tables::entry_symbols {', 'using Routine = int32_t(AllRegs&);']
    for name,segment,offset in labels:
        if (segment,int(offset)) in entries:
            symbols.append(f'inline constexpr guest::EntrySymbol<Routine> {name}(guest::SegmentId(uint16_t(Segment::{segment})), {offset});')
    symbols += ['}']
    outputs[ROOT/'src/tables/guest_labels.h']='\n'.join(symbols)+'\n'
    errors=[]
    for p,text in outputs.items():
        if check:
            if not p.exists() or p.read_text()!=text:errors.append(str(p.relative_to(ROOT)))
        else:p.write_text(text)
    if errors:raise ValueError('stale static segment metadata: '+', '.join(errors))
    print(f'{len(segments)} segments, {len(relocations)} typed relocations: '+('verified' if check else 'generated'))
if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--check',action='store_true')
    generate(parser.parse_args().check)
