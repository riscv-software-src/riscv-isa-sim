# Widening multiply-add instructions read vd as a 2*SEW-wide source, so their
# SEW-wide vector sources must not overlap vd at all: that would read the same
# register with two different EEWs, which is reserved.  This includes the
# upper-half overlap that non-accumulating widening instructions may use.
#
# Each case expects either an illegal-instruction trap (2) or no trap (0).
# All cases run; tohost reports the number of the first case that failed.

  .macro check expected, insn:vararg
  addi s0, s0, 1
  li s1, 0
  \insn
  li t0, \expected
  beq s1, t0, 1f
  bnez s2, 1f
  mv s2, s0
1:
  .endm

  # Unmasked OP-V encoding, for instructions the assembler may not know
  # (Zvabd, Zvfbfwma).  src1 is the vs1/rs1 field.
  .macro opv funct6, funct3, vd, vs2, src1
  .word (\funct6 << 26) | (1 << 25) | (\vs2 << 20) | (\src1 << 15) | (\funct3 << 12) | (\vd << 7) | 0x57
  .endm

  .section .text.init
  .globl _start
_start:
  la t0, trap
  csrw mtvec, t0
  li t0, (1 << 9) | (1 << 13)  # mstatus.VS and mstatus.FS = Initial
  csrs mstatus, t0
  li s0, 0
  li s2, 0
  li a0, 0

  vsetivli x0, 4, e8, m1, ta, ma
  check 2, vwmaccu.vv v10, v11, v1
  check 2, vwmaccu.vv v10, v1, v11
  check 2, vwmacc.vv v10, v11, v1
  check 2, vwmacc.vv v10, v1, v11
  check 2, vwmaccsu.vv v10, v11, v1
  check 2, vwmaccsu.vv v10, v1, v11
  check 2, vwmaccu.vx v10, a0, v11
  check 2, vwmacc.vx v10, a0, v11
  check 2, vwmaccsu.vx v10, a0, v11
  check 2, vwmaccus.vx v10, a0, v11
  check 2, opv 0x3d, 0, 10, 11, 1   # vwabda.vv v10, v11, v1
  check 2, opv 0x3e, 0, 10, 1, 11   # vwabdau.vv v10, v1, v11
  check 2, opv 0x3d, 4, 10, 11, 10  # vwabda.vx v10, v11, a0
  check 0, vwmaccu.vv v10, v1, v2
  check 0, vwmaccu.vx v10, a0, v2
  check 0, opv 0x3d, 0, 10, 2, 1    # vwabda.vv v10, v2, v1
  # Non-accumulating widening ops may overlap the upper half of vd.
  check 0, vwaddu.vv v10, v11, v1
  check 0, vwmul.vv v10, v1, v11
  check 0, vwaddu.vx v10, v11, a0

  vsetivli x0, 4, e8, m2, ta, ma
  check 2, vwmaccu.vv v8, v10, v2
  check 2, vwmacc.vx v8, a0, v10
  check 0, vwaddu.vv v8, v10, v2

  vsetivli x0, 4, e32, m1, ta, ma
  check 2, vfwmacc.vv v10, v11, v1
  check 2, vfwmacc.vv v10, v1, v11
  check 2, vfwnmacc.vv v10, v11, v1
  check 2, vfwmsac.vv v10, v11, v1
  check 2, vfwnmsac.vv v10, v11, v1
  check 2, vfwmacc.vf v10, fa0, v11
  check 2, vfwnmacc.vf v10, fa0, v11
  check 2, vfwmsac.vf v10, fa0, v11
  check 2, vfwnmsac.vf v10, fa0, v11
  check 0, vfwmacc.vv v10, v1, v2
  check 0, vfwadd.vv v10, v11, v1
  check 0, vfwmul.vv v10, v1, v11
  check 0, vfwsub.vf v10, v11, fa0

  vsetivli x0, 4, e16, m1, ta, ma
  check 2, opv 0x3b, 1, 10, 1, 11   # vfwmaccbf16.vv v10, v11, v1
  check 2, opv 0x3b, 1, 10, 11, 1   # vfwmaccbf16.vv v10, v1, v11
  check 2, opv 0x3b, 5, 10, 11, 10  # vfwmaccbf16.vf v10, fa0, v11
  check 0, opv 0x3b, 1, 10, 2, 1    # vfwmaccbf16.vv v10, v1, v2

  li t1, 1
  beqz s2, exit
  slli t1, s2, 1
  ori t1, t1, 1
exit:
  la t0, tohost
  sd t1, 0(t0)
2:
  j 2b

trap:
  csrr s1, mcause
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  mret

.section .tohost,"aw",@progbits
.balign 64
.globl tohost
tohost:
  .dword 0
.balign 64
.globl fromhost
fromhost:
  .dword 0
