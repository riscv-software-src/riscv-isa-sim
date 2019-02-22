.text
.align 2

.globl vec_saxpy_asm
.type  vec_saxpy_asm,@function

# assumes calling convention:
#     a0      n
#     fa0     a
#     a1      x
#     a2      y

vec_saxpy_asm:
    vsetvli a3, a0, e32,m8
    vlw.v v0, (a1)
    sub a0, a0, a3
    slli a3, a3, 2
    add a1, a1, a3
    vlw.v v8, (a2)
    vfmacc.vf v8, v0, fa0
    vsw.v v8, (a2)
    add a2, a2, a3
    bnez a0, vec_saxpy_asm
    ret
