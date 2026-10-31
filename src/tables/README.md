# The tables

What AMOS's assembler sources held besides code: the data arrays (the token
tables, the jump tables, the messages, the default mouse bank, the libraries'
images), the relocations of their addresses, and the entries of the glue
(`GlueEntry`, `src/runtime/glue.h`). They were converted to C++ once, from
the assembler, and are the source now: nothing generates them.

- one file for each assembler source (`b.cpp` was +B.s, `w.cpp` +W.s,
  `edit.cpp` +Edit.s, `lib_dos_glue.cpp` src/lib/dos_glue.s...): its bytes,
  `relocate_<name>()` (the addresses written into them at start-up) and
  `entries_<name>` (its entries);
- `labels.h`/`labels.cpp`: the labels the C++ uses, as constants that point
  into the arrays;
- `index.cpp`: the lists of entries and the order of the relocations;
- `w_direct.cpp`: the functions of AMOS.library's vector tables by number;
- `tables.h`: what the files share.

The numbers of the library's routines are `src/core/lib_routines.h`; the
tables' offsets must not change (the `.AMOS` programs' tokens are offsets in
the token tables).
