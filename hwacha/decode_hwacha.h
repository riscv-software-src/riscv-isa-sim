#ifndef _DECODE_HWACHA_H
#define _DECODE_HWACHA_H

#define XS1 (xs1)
#define XS2 (xs2)
#define WRITE_XRD(value) (xd = value)

#define NXPR (h->get_ct_state()->nxpr)
#define NFPR (h->get_ct_state()->nfpr)
#define MAXVL (h->get_ct_state()->maxvl)
#define VL (h->get_ct_state()->vl)
#define WRITE_NXPR(nxprnext) (h->get_ct_state()->nxpr = (nxprnext))
#define WRITE_NFPR(nfprnext) (h->get_ct_state()->nfpr = (nfprnext))
#define WRITE_MAXVL(maxvlnext) (h->get_ct_state()->maxvl = (maxvlnext))
#define WRITE_VL(vlnext) (h->get_ct_state()->vl = (vlnext))

#define INSN_RS1 (insn.rs1())
#define INSN_RS2 (insn.rs2())
#define INSN_RS3 (insn.rs3())
#define INSN_RD (insn.rd())
#define INSN_SEG ((insn.i_imm() >> 9)+1)

#define UT_READ_XPR(idx, src) (h->get_ut_state(idx)->XPR[src])
#define UT_WRITE_XPR(idx, dst, value) (h->get_ut_state(idx)->XPR.write(dst, value))
#define UT_RS1(idx) (UT_READ_XPR(idx, INSN_RS1))
#define UT_RS2(idx) (UT_READ_XPR(idx, INSN_RS2))
#define UT_WRITE_RD(idx, value) (UT_WRITE_XPR(idx, INSN_RD, value))

#define UT_READ_FPR(idx, src) (h->get_ut_state(idx)->FPR[src])
#define UT_WRITE_FPR(idx, dst, value) (h->get_ut_state(idx)->FPR.write(dst, value))
#define UT_FRS1(idx) (UT_READ_FPR(idx, INSN_RS1))
#define UT_FRS2(idx) (UT_READ_FPR(idx, INSN_RS2))
#define UT_FRS3(idx) (UT_READ_FPR(idx, INSN_RS3))
#define UT_WRITE_FRD(idx, value) (UT_WRITE_FPR(idx, INSN_RD, value))

#define VEC_SEG_LOAD(dst, func, inc) \
  VEC_SEG_ST_LOAD(dst, func, INSN_SEG*inc, inc)

#define VEC_SEG_ST_LOAD(dst, func, stride, inc) \
  reg_t seg_addr = XS1; \
  for (uint32_t i=0; i<VL; i++) { \
    reg_t addr = seg_addr; \
    seg_addr += stride; \
    for (uint32_t j=0; j<INSN_SEG; j++) { \
      UT_WRITE_##dst(i, INSN_RD+j, p->get_mmu()->func(addr)); \
      addr += inc; \
    } \
  }

#define VEC_SEG_STORE(src, func, inc) \
  VEC_SEG_ST_STORE(src, func, INSN_SEG*inc, inc)

#define VEC_SEG_ST_STORE(src, func, stride, inc) \
  reg_t seg_addr = XS1; \
  for (uint32_t i=0; i<VL; i++) { \
    reg_t addr = seg_addr; \
    seg_addr += stride; \
    for (uint32_t j=0; j<INSN_SEG; j++) { \
      p->get_mmu()->func(addr, UT_READ_##src(i, INSN_RD+j)); \
      addr += inc; \
    } \
  }

#endif
