// See LICENSE for license details.
// Check reserved source EEW overlaps as well as the legal destination/source
// overlaps that must not be rejected. Emit raw encodings so the assembler
// cannot reject or rewrite the reserved operands being tested.
#include "../riscv/encoding.h"

.option norvc
.option norelax
.section .text.init
.globl _start
_start:
  li s0, 0
  li s1, 0
  la t0, trap
  csrw mtvec, t0
  li t0, MSTATUS_VS | MSTATUS_FS
  csrs mstatus, t0
  fcvt.d.w f0, zero
  la a0, scratch
  li t0, -1
  vsetvli t0, t0, e8, m8, tu, mu
  vmv.v.i v0, 0
  vmv.v.i v8, 0
  vmv.v.i v16, 0
  vmv.v.i v24, 0

  // On failure, Spike reports the test number in tohost. A trap must be an
  // illegal instruction at exactly the word under test, not an access fault
  // or a trap in the test setup. s2 records whether that trap was observed.
  .macro check op, vd, vs2, vs1=0, vm=1, illegal=0, nf=1
    addi s0, s0, 1
    li s1, \illegal
    li s2, 0
    la s3, 1f
    csrr s4, vstart
  1:
    .word (\op & ~(1 << 25)) | (\vd << 7) | (\vs1 << 15) | (\vs2 << 20) | (\vm << 25) | ((\nf - 1) << 29)
    bne s1, s2, fail
  .endm

  // Both vector sources, a single vector source, mask destinations, carry
  // inputs, and merge selectors. v0 is initially all zero: even completely
  // masked-off instructions must check the encoding before their loops.
  vsetivli zero, 1, e16, m1, tu, mu
  check MATCH_VADD_VV, 8, 0, 2, 0, 1
  check MATCH_VADD_VV, 8, 4, 0, 0, 1
  check MATCH_VADD_VV, 8, 4, 4, 0
  check MATCH_VADD_VV, 8, 0, 0
  check MATCH_VADD_VX, 8, 0, 0, 0, 1
  check MATCH_VADD_VI, 8, 0, 0, 0, 1
  check MATCH_VADD_VX, 8, 4, 0, 0
  check MATCH_VMSEQ_VV, 0, 4, 2, 0
  check MATCH_VMSEQ_VV, 8, 0, 2, 0, 1
  check MATCH_VMADC_VVM, 8, 0, 2, 0, 1
  check MATCH_VMADC_VVM, 0, 4, 2, 0
  check MATCH_VMADC_VV, 8, 0, 2
  check MATCH_VADC_VVM, 8, 0, 2, 0, 1
  check MATCH_VADC_VVM, 8, 4, 0, 0, 1
  check MATCH_VMERGE_VVM, 8, 0, 2, 0, 1
  check MATCH_VMERGE_VVM, 8, 4, 0, 0, 1
  check MATCH_VMERGE_VXM, 8, 0, 0, 0, 1
  check MATCH_VMERGE_VIM, 8, 0, 0, 0, 1
  check MATCH_VFMERGE_VFM, 8, 0, 0, 0, 1
  check MATCH_VMERGE_VVM, 8, 4, 2, 0
  // Move encodings have no selector mask or vs2 operand, despite sharing
  // merge implementation macros. Their fixed vs2=v0 must remain legal.
  check MATCH_VMV_V_V, 8, 0, 0
  check MATCH_VMV_V_X, 0, 0, 0
  check MATCH_VMV_V_I, 0, 0, 0
  check MATCH_VFMV_V_F, 0, 0, 0
  check MATCH_VFADD_VV, 8, 0, 2, 0, 1
  check MATCH_VFADD_VF, 8, 0, 0, 0, 1
  check MATCH_VMFEQ_VV, 0, 4, 2, 0
  check MATCH_VMFEQ_VF, 8, 0, 0, 0, 1
  check MATCH_VFWCVT_F_F_V, 8, 0, 0, 0, 1
  check MATCH_VFWCVT_F_F_V, 8, 9
  check MATCH_VFNCVT_F_F_W, 8, 0, 0, 0, 1
  check MATCH_VFNCVT_F_F_W, 8, 8

  // All widening accumulator instructions read vd at 2*SEW. Test the upper
  // half (legal for a write-only widening instruction), each source, and a
  // disjoint control. These also cover FP and BF16 loop wrappers.
  .macro acc_vv op
    check \op, 8, 9, 2, 1, 1
    check \op, 8, 4, 9, 1, 1
    check \op, 8, 4, 2
  .endm
  .macro acc_vx op
    check \op, 8, 9, 0, 1, 1
    check \op, 8, 4, 0
  .endm
  .irp op, MATCH_VWMACC_VV, MATCH_VWMACCU_VV, MATCH_VWMACCSU_VV, MATCH_VFWMACC_VV, MATCH_VFWNMACC_VV, MATCH_VFWMSAC_VV, MATCH_VFWNMSAC_VV, MATCH_VFWMACCBF16_VV, MATCH_VWABDA_VV, MATCH_VWABDAU_VV
    acc_vv \op
  .endr
  .irp op, MATCH_VWMACC_VX, MATCH_VWMACCU_VX, MATCH_VWMACCSU_VX, MATCH_VWMACCUS_VX, MATCH_VFWMACC_VF, MATCH_VFWNMACC_VF, MATCH_VFWMSAC_VF, MATCH_VFWNMSAC_VF, MATCH_VFWMACCBF16_VF, MATCH_VWABDA_VX, MATCH_VWABDAU_VX
    acc_vx \op
  .endr
  check MATCH_VWMUL_VV, 8, 9, 4
  check MATCH_VFWADD_VV, 8, 9, 4
  check MATCH_VWMUL_VV, 8, 8, 4, 1, 1
  check MATCH_VWMACC_VV, 8, 0, 2, 0, 1
  check MATCH_VWMACC_VV, 8, 4, 2, 0
  vsetivli zero, 1, e16, m2, tu, mu
  check MATCH_VWMACC_VV, 8, 10, 4, 1, 1
  check MATCH_VWMUL_VV, 8, 10, 4
  vsetivli zero, 1, e16, mf2, tu, mu
  check MATCH_VWMACC_VV, 8, 8, 4, 1, 1
  check MATCH_VWMUL_VV, 8, 8, 4, 1, 1
  check MATCH_VWMACC_VV, 8, 4, 2

  // Mixed-width arithmetic sources: wide .wv and narrowing .wv forms.
  vsetivli zero, 1, e16, m2, tu, mu
  check MATCH_VWADD_WV, 16, 8, 10, 1, 1
  check MATCH_VWADD_WV, 16, 8, 4
  check MATCH_VWADD_WV, 8, 8, 4
  check MATCH_VWADD_WX, 8, 8
  check MATCH_VWADD_WV, 16, 0, 4, 0, 1
  check MATCH_VNSRL_WV, 16, 8, 10, 1, 1
  check MATCH_VNSRL_WV, 8, 8, 4
  check MATCH_VNSRL_WV, 10, 8, 4, 1, 1
  check MATCH_VNSRL_WX, 8, 8
  check MATCH_VNSRL_WI, 8, 8
  check MATCH_VNSRL_WV, 16, 0, 4, 0, 1
  vsetivli zero, 1, e16, mf2, tu, mu
  check MATCH_VNSRL_WV, 16, 8, 8, 1, 1
  check MATCH_VNSRL_WV, 8, 8, 4
  check MATCH_VWADD_WV, 16, 8, 8, 1, 1

  // Reduction scalar inputs have EMUL=1, independent of LMUL. Only the
  // destination, not either data source, may overlap the execution mask.
  vsetivli zero, 1, e16, m2, tu, mu
  check MATCH_VREDSUM_VS, 0, 4, 3, 0
  check MATCH_VREDSUM_VS, 4, 4, 5
  check MATCH_VREDSUM_VS, 8, 0, 3, 0, 1
  check MATCH_VREDSUM_VS, 8, 4, 0, 0, 1
  check MATCH_VWREDSUM_VS, 8, 4, 5, 1, 1
  check MATCH_VWREDSUM_VS, 4, 4, 3
  check MATCH_VWREDSUM_VS, 0, 4, 3, 0
  check MATCH_VFREDOSUM_VS, 0, 4, 3, 0
  check MATCH_VFREDUSUM_VS, 8, 4, 0, 0, 1
  check MATCH_VFWREDOSUM_VS, 8, 4, 5, 1, 1
  check MATCH_VFWREDUSUM_VS, 4, 4, 3
  vsetivli zero, 1, e16, mf2, tu, mu
  check MATCH_VWREDSUM_VS, 8, 4, 4, 1, 1
  check MATCH_VWREDSUM_VS, 8, 4, 5
  vsetivli zero, 1, e8, m8, tu, mu
  check MATCH_VWREDSUM_VS, 31, 8, 31
  check MATCH_VWREDSUM_VS, 31, 8, 15, 1, 1

  // Explicit masks, and gather indices with a different EEW. Include
  // aliases in the interior of groups and fractional-EMUL aliases.
  vsetivli zero, 1, e16, m2, tu, mu
  check MATCH_VCOMPRESS_VM, 8, 4, 5, 1, 1
  check MATCH_VCOMPRESS_VM, 8, 4, 3
  check MATCH_VCOMPRESS_VM, 8, 0, 3
  check MATCH_VCOMPRESS_VM, 8, 4, 8, 1, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 4
  check MATCH_VRGATHEREI16_VV, 8, 0, 4, 0, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 0, 0, 1
  check MATCH_VRGATHER_VV, 8, 0, 4, 0, 1
  check MATCH_VRGATHER_VX, 8, 0, 0, 0, 1
  check MATCH_VRGATHER_VI, 8, 0, 0, 0, 1
  check MATCH_VRGATHER_VV, 8, 4, 4
  check MATCH_VRGATHER_VV, 8, 8, 4, 1, 1
  vsetivli zero, 1, e8, m1, tu, mu
  check MATCH_VRGATHEREI16_VV, 8, 5, 4, 1, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 2
  vsetivli zero, 1, e32, m2, tu, mu
  check MATCH_VRGATHEREI16_VV, 8, 4, 5, 1, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 3
  vsetivli zero, 1, e8, mf8, tu, mu
  check MATCH_VRGATHEREI16_VV, 8, 4, 4, 1, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 5
  check MATCH_VCOMPRESS_VM, 8, 4, 4, 1, 1

  // Mask-only sources may alias v0; they have EEW=1 too.
  vsetivli zero, 1, e16, m1, tu, mu
  check MATCH_VMAND_MM, 0, 0, 0
  check MATCH_VCPOP_M, 5, 0, 0, 0
  check MATCH_VFIRST_M, 5, 0, 0, 0
  check MATCH_VIOTA_M, 8, 0, 0, 0
  check MATCH_VMSBF_M, 8, 0, 0, 0
  check MATCH_VMSIF_M, 8, 0, 0, 0
  check MATCH_VMSOF_M, 8, 0, 0, 0
  check MATCH_VSLIDEUP_VI, 8, 0, 0, 0, 1
  check MATCH_VSLIDEDOWN_VI, 8, 0, 0, 0, 1
  check MATCH_VSLIDEDOWN_VI, 4, 4
  check MATCH_VFSLIDE1DOWN_VF, 8, 0, 0, 0, 1
  check MATCH_VZEXT_VF2, 8, 0, 0, 0, 1
  check MATCH_VZEXT_VF2, 8, 4
  vsetivli zero, 1, e16, m2, tu, mu
  check MATCH_VZEXT_VF2, 8, 9
  check MATCH_VZEXT_VF2, 8, 8, 0, 1, 1
  vsetivli zero, 1, e8, m1, tu, mu
  check MATCH_VFNCVT_F_F_Q, 8, 0, 0, 0, 1
  check MATCH_VFNCVT_F_F_Q, 8, 8
  check MATCH_VFNCVT_F_F_Q, 9, 8, 0, 1, 1

  // Memory operands: index EEW is encoded, data EEW is SEW for indexed
  // operations. A segment's later fields are also sources for stores.
  vsetivli zero, 1, e32, m1, tu, mu
  check MATCH_VSE8_V, 0, 0, 10, 0, 1
  check MATCH_VSE32_V, 0, 0, 10, 0, 1
  check MATCH_VSSE16_V, 0, 0, 10, 0, 1
  check MATCH_VSE8_V, 0, 0, 10
  check MATCH_VSE32_V, 4, 0, 10, 0
  check MATCH_VSM_V, 0, 0, 10
  check MATCH_VLUXEI16_V, 8, 0, 10, 0, 1
  check MATCH_VLOXEI16_V, 8, 0, 10, 0, 1
  check MATCH_VLUXEI16_V, 8, 0, 10
  check MATCH_VSUXEI16_V, 0, 4, 10, 0, 1
  check MATCH_VSUXEI16_V, 4, 0, 10, 0, 1
  check MATCH_VSUXEI16_V, 4, 4, 10, 1, 1
  check MATCH_VSUXEI32_V, 4, 4, 10
  check MATCH_VSUXEI16_V, 4, 5, 10, 1, 1, 2
  check MATCH_VSOXEI16_V, 4, 5, 10, 1, 1, 2
  check MATCH_VSUXEI32_V, 4, 5, 10, 1, 0, 2
  check MATCH_VSUXEI16_V, 4, 7, 10, 1, 0, 2
  // Overlap must trap even when vstart >= vl (memory instructions support
  // nonzero vstart). Leave a distinctive vstart for the handler to verify.
  li t0, 1
  csrw vstart, t0
  check MATCH_VSUXEI16_V, 4, 5, 10, 1, 1, 2
  csrw vstart, zero
  vsetivli zero, 1, e16, m1, tu, mu
  check MATCH_VSUXEI32_V, 5, 4, 10, 1, 1
  check MATCH_VLUXEI32_V, 4, 4, 10
  check MATCH_VLUXEI32_V, 5, 4, 10, 1, 1
  check MATCH_VLUXEI16_V, 4, 4, 10
  check MATCH_VLUXEI16_V, 4, 5, 10, 1, 1, 2
  vsetivli zero, 1, e16, mf2, tu, mu
  check MATCH_VSUXEI8_V, 4, 5, 10, 1, 1, 2
  check MATCH_VSUXEI16_V, 4, 5, 10, 1, 0, 2

  // The rules are about instruction operands, not the elements executed.
  // Also exercise the maximum element width and a larger source group.
  vsetivli zero, 1, e64, m4, tu, mu
  check MATCH_VADD_VV, 8, 0, 4, 0, 1
  check MATCH_VADD_VV, 8, 4, 4, 0
  check MATCH_VRGATHEREI16_VV, 8, 4, 7, 1, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 3
  vsetivli zero, 0, e16, m1, tu, mu
  check MATCH_VADD_VV, 8, 0, 2, 0, 1
  check MATCH_VWMACC_VV, 8, 9, 2, 1, 1
  check MATCH_VRGATHEREI16_VV, 8, 4, 4
  check MATCH_VSUXEI8_V, 4, 4, 10, 1, 1

  // Extension-specific shared checks. The scalar dot-product destination
  // and broadcast-dot sources have different group shapes from ordinary V.
  vsetivli zero, 1, e8, m1, tu, mu
  check MATCH_VZIP_VV, 8, 0, 4, 0, 1
  check MATCH_VZIP_VV, 8, 4, 2, 0
  check MATCH_VUNZIPE_V, 8, 0, 0, 0, 1
  check MATCH_VPAIRE_VV, 8, 4, 0, 0, 1
  check MATCH_VQWDOTAU_VV, 8, 0, 4, 0, 1
  check MATCH_VQWDOTAU_VV, 8, 4, 2, 0
  check MATCH_VQWBDOTAU_VV, 16, 0, 8, 0, 1
  check MATCH_VQWBDOTAU_VV, 16, 8, 0, 0, 1
  check MATCH_VQWBDOTAU_VV, 16, 0, 8
  check MATCH_VQWBDOTAU_VV, 16, 8, 4, 0

  li t1, 1
  j exit

.balign 4
trap:
  csrr t0, mcause
  li t1, CAUSE_ILLEGAL_INSTRUCTION
  bne t0, t1, fail
  beqz s1, fail
  csrr t0, mepc
  bne t0, s3, fail
  csrr t1, vstart
  bne t1, s4, fail
  li s2, 1
  addi t0, t0, 4
  csrw mepc, t0
  mret

fail:
  li t1, 3
  beqz s0, exit
  slli t1, s0, 1
  ori t1, t1, 1
exit:
  la t0, tohost
  sw t1, 0(t0)
1:
  j 1b

.section .data
.balign 64
scratch:
  .zero 4096

.section .tohost, "aw", @progbits
.balign 64
.globl tohost
tohost:
  .dword 0
.globl fromhost
fromhost:
  .dword 0
