/*======================================================================*/
/* pmp testlinker script                                             */
/*======================================================================*/

MEMORY
{
    /* 
     * default location 
     * NAPOT is used. Thus size must be 2^n.
     * For easier handling, esp. when MML set, trap handler has separated stack at GP
     * - Currently allocate 0x140000. Just avoid overlap with _end.
     */
    M_MEM (AX)  : ORIGIN = 0x100000, LENGTH = 1M
            
    RESERVED  : ORIGIN = 0x000000, LENGTH = 1M
    
    TEST_MEM (AX)  : ORIGIN = 0x200000, LENGTH = 256K
    
    U_MEM (AX)  : ORIGIN = 0x240000, LENGTH = 64K
}

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

/*----------------------------------------------------------------------*/
/* Sections                                                             */
/*----------------------------------------------------------------------*/

SECTIONS
{
    . = 0x100000;
    __global_pointer$ = 0x140000;
    
    .text : ALIGN(256) { 
        *(.text.init) 
        *(.text) *(.text.*) *(.device_code) . = ALIGN(256); . = 0x10000; }
    PROVIDE (__TEXT_END = .);
    
    .tohost : { *(.tohost) }
    .rodata : ALIGN(256) { *(.rodata) *(.rodata.*) }
    .sdata  : ALIGN(256) {
      *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
      *(.sdata .sdata.* .gnu.linkonce.s.*)
    }
    
    /* data sections, mostly for host to deal with input and output data */
    .data : {LONG(0xdeadbeef) *(.data) }

    /* bss segment */
    .sbss : {
      *(.sbss .sbss.* .gnu.linkonce.sb.*)
      *(.scommon)
    }
    .bss : { *(.bss) }
    
    /* 
     * thread-local data segment. 
     * Copied to TCM<n> at start by init_tls().
     */
    .tdata : ALIGN(256)
    {
      _tdata_begin = .;
      *(.tls_start) *(.tdata) *(.tdata.*) 
      _tdata_end = .;
    }
    .tbss :
    {
      *(.tbss) *(.tbss.*)
      _tbss_end = .;
    }
    
    /* End of uninitalized data segement, as the start for 128KB stack + TLS */
    _end = 0x180000;
    
    .umode : { *(.text_umode) *(.data_umode) } > U_MEM
    
    .test : { *(.text_test_foo) *(.data_test_arr) } > TEST_MEM
}

