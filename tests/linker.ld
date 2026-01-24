ENTRY(_start)

/* Single RAM region */
MEMORY
{
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128K
}

SECTIONS
{
  .text : {
    *(.text*)
  } > RAM

  .rodata : {
    *(.rodata*)
  } > RAM

  .data : {
    *(.data*)
  } > RAM

  .bss : {
    *(.bss*)
    *(COMMON)
  } > RAM

  /* Stack grows downward */
  _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
