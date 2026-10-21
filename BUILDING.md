# Building AMOS Professional on a Unix host

Requirements: the Amiga cross toolchain from
https://github.com/AmigaPorts/m68k-amigaos-gcc (installed in `/opt/amiga`, or
pass `AMIGA_PREFIX=...`), GNU make and python3.

```
make            # assemble everything into build/
make verify     # compare with the prebuilt binaries shipped in AMOS/
make dist       # build/dist/AMOS: the AMOS tree with the new binaries
```

## What replaces the original Amiga tools

| Original (Amiga)              | Here                         |
|-------------------------------|------------------------------|
| `c/Genam`, `c/genim2`         | `vasmm68k_mot -devpac`        |
| `c/Make_Labels`               | `tools/make_labels.py`        |
| `c/Make_Toktable`             | `tools/make_toktable.py`      |
| `c/Library_Digest`            | `tools/library_digest.py`     |
| `c/Get_Chunk`                 | `vasm -Fbin` plus longword padding |
| AmigaDOS case-insensitive names | `tools/case_links.py` (symlinks in `build/ci`) |

Generated files go to `build/gen`; nothing is written into the source tree.

## Assembler compatibility notes

- `+W.s` (AMOS.library) was built with GenIm2, which matches symbols without
  regard to case, so it is assembled with `-nocase`. All other sources need
  case-sensitive symbols (they contain e.g. both `.skip` and `.Skip`).
- `EzFlag` (Easy AMOS variant of AMOS.library) is not defined in the sources
  and is passed as `-DEzFlag=0`.
- `-kick1hunks` keeps relocations in `HUNK_RELOC32`, as GenAm did, so the
  binaries still load on Kickstart 1.3.

## Verification

`tools/bincmp.py` compares hunk files semantically. It accepts the encoding
choices on which GenAm and vasm differ (`andi` vs `and #imm`, the order of `exg`
operands, padding, memory flags in the hunk id, reloc order) and reports
everything else. All nine programs and libraries are equivalent to the
prebuilt ones, and the two config files are byte-identical.

Three source fixes were needed:
- `+B.s`: `bne.` is now `bne`, assembled as `bne.w` as before.
- `+Edit.s`: `cmp.s` is now `cmp.b`.
- `+Editor_Config.s`: the `ç`/`©` bytes in two strings had been replaced by
  U+FFFD in an earlier re-encoding of the repository, and are restored.
