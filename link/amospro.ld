/* Linker script for the AMOSPro main program.
   Hunk order matters: "E" holds the entry point and must be the first hunk;
   "I" (initialisation code) must directly follow it, because Init_Fin in
   +B.s unlinks and frees the second hunk once AMOS is running. Code and data
   from C/C++ objects come after them. */
OUTPUT_FORMAT("amiga")
OUTPUT_ARCH(m68k)
SECTIONS
{
  . = 0x0;
  E : { *(E) }
  I : { *(I) }
  .text :
  {
    *(.text) *(.text.*) *(_*) *(.rodata*) *(.data.rel.ro*)
    *(.gnu.linkonce.t.*) *(.gnu.linkonce.r.*)
  }
  .data :
  {
    *(.data) *(.data.*) *(.gnu.linkonce.d.*)
  }
  .bss :
  {
    *(.bss) *(.bss.*) *(COMMON)
  }
  .datachip : { *(.datachip) }
  .bsschip : { *(.bsschip) }
  /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) *(.gcc_except_table*) }
}
