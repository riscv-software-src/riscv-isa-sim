#ifndef _HWACHA_H
#define _HWACHA_H

// vector stuff
#define VL vl

#define UT_RS1(idx) uts[idx]->XPR[insn.rtype.rs1]
#define UT_RS2(idx) uts[idx]->XPR[insn.rtype.rs2]
#define UT_RD(idx) uts[idx]->XPR.write_port(insn.rtype.rd)
#define UT_RA(idx) uts[idx]->XPR.write_port(1)
#define UT_FRS1(idx) uts[idx]->FPR[insn.ftype.rs1]
#define UT_FRS2(idx) uts[idx]->FPR[insn.ftype.rs2]
#define UT_FRS3(idx) uts[idx]->FPR[insn.ftype.rs3]
#define UT_FRD(idx) uts[idx]->FPR.write_port(insn.ftype.rd)
#define UT_RM(idx) ((insn.ftype.rm != 7) ? insn.ftype.rm : \
              ((uts[idx]->fsr & FSR_RD) >> FSR_RD_SHIFT))

#define UT_LOOP_START for (int i=0;i<VL; i++) {
#define UT_LOOP_END }
#define UT_LOOP_RS1 UT_RS1(i)
#define UT_LOOP_RS2 UT_RS2(i)
#define UT_LOOP_RD UT_RD(i)
#define UT_LOOP_RA UT_RA(i)
#define UT_LOOP_FRS1 UT_FRS1(i)
#define UT_LOOP_FRS2 UT_FRS2(i)
#define UT_LOOP_FRS3 UT_FRS3(i)
#define UT_LOOP_FRD UT_FRD(i)
#define UT_LOOP_RM UT_RM(i)

#define VEC_LOAD(dst, func, inc) \
  reg_t addr = RS1; \
  UT_LOOP_START \
    UT_LOOP_##dst = mmu.func(addr); \
    addr += inc; \
  UT_LOOP_END

#define VEC_STORE(src, func, inc) \
  reg_t addr = RS1; \
  UT_LOOP_START \
    mmu.func(addr, UT_LOOP_##src); \
    addr += inc; \
  UT_LOOP_END

enum vt_command_t
{
  vt_command_stop,
};

#endif
