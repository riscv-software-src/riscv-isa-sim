OUTPUT_ARCH( "riscv" )
ENTRY( entry )
SECTIONS
{
    . = 0xfffff800;     /* TODO: 0x800 */
    .text :
    {
        *(.text)
    }
    _end = .;
}
