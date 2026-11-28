ENTRY(_start)

SECTIONS
{
  . = 0x1000;
  .ebreak : { KEEP(*(.ebreak)) }

  . = 0x1100;
  .text : { *(.text.start) *(.text.handler) }
  .rodata : { *(.rodata) }
}