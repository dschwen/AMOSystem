/* Linker script for the programs AMOSPro loads with LoadSeg: AMOS.library,
   AMOSPro_Editor and AMOSPro_Monitor. Their assembler code (vasm's default
   section "CODE") must be the first hunk: it starts with the entry table
   AMOSPro reads. AMOS.library's chip-memory data ("m") follows; the linker
   only marks a hunk as chip memory when the output section is named
   .datachip. Code and data from C/C++ objects come last. */
OUTPUT_FORMAT("amiga")
OUTPUT_ARCH(m68k)
SECTIONS
{
  . = 0x0;
  CODE : { *(CODE) }
  .datachip : { *(m) *(.datachip) }
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
  .bsschip : { *(.bsschip) }
  /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) *(.gcc_except_table*) }
}
