.text
.global _start
_start:
    la gp, __global_pointer$
    la sp, __stack_bottom
    j pre_main
.bss
    .align 2
.global __stack_top
__stack_top:
    .zero 0x100000
.global __stack_bottom
__stack_bottom:
