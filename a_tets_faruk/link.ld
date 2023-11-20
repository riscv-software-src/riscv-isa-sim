ENTRY(_start)

MEMORY
{
    RAM (wxa!ri) : ORIGIN = 0x81000000, LENGTH = 128K
    ROM (rx)     : ORIGIN = 0x80000000, LENGTH = 128K
}

SECTIONS
{
    .text : {
        *(.text)
    } > ROM

    .rodata : {
        *(.rodata*)
    } > ROM

    .data : {
        *(.data)
    } > RAM

    .bss : {
        *(.bss)
    } > RAM

    .htif : {
        PROVIDE_HIDDEN(tohost = .);
        . += 8;
        PROVIDE_HIDDEN(fromhost = .);
        . += 8;
    } > RAM

    . = ALIGN(8);
    _end = .;
}
