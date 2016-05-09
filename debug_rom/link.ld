OUTPUT_ARCH( "riscv" )
ENTRY( entry )
SECTIONS
{
    . = 0x800;
    .text :
    {
        *(.text)
    }
    _end = .;
}
