/* Linker script for the AMOSPro main program: the code begins with the
   program's entry (src/amiga/start.cpp, .text.entry). */
OUTPUT_FORMAT("amiga")
OUTPUT_ARCH(m68k)
SECTIONS
{
  . = 0x0;
  .text :
  {
    *(.text.entry) *(CODE) *(.text) *(.text.*) *(_*) *(.rodata*) *(.data.rel.ro*)
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
  .datachip : { *(m) *(.datachip) }
  .bsschip : { *(.bsschip) }
  /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) *(.gcc_except_table*) }
}
