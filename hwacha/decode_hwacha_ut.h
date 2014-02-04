#ifndef _DECODE_HWACHA_UT_H
#define _DECODE_HWACHA_UT_H

#include "decode.h"
#include "decode_hwacha.h"
#include "hwacha.h"
#include "hwacha_xcpt.h"

#undef RS1
#undef RS2
#undef WRITE_RD

static inline reg_t read_rs1(hwacha_t* h, insn_t insn, uint32_t idx)
{
  if (INSN_RS1 >= h->get_ct_state()->nxpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  return UT_RS1(idx);
}

static inline reg_t read_rs2(hwacha_t* h, insn_t insn, uint32_t idx)
{
  if (INSN_RS2 >= h->get_ct_state()->nxpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  return UT_RS2(idx);
}

static inline void write_rd(hwacha_t* h, insn_t insn, uint32_t idx, reg_t value)
{
  if (INSN_RD >= h->get_ct_state()->nxpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  UT_WRITE_RD(idx, value);
}

#define RS1 read_rs1(h, insn, UTIDX)
#define RS2 read_rs2(h, insn, UTIDX)
#define WRITE_RD(value) write_rd(h, insn, UTIDX, value)

#undef FRS1
#undef FRS2
#undef FRS3
#undef WRITE_FRD

static inline reg_t read_frs1(hwacha_t* h, insn_t insn, uint32_t idx)
{
  if (INSN_RS1 >= h->get_ct_state()->nfpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  return UT_FRS1(idx);
}

static inline reg_t read_frs2(hwacha_t* h, insn_t insn, uint32_t idx)
{
  if (INSN_RS2 >= h->get_ct_state()->nfpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  return UT_FRS2(idx);
}

static inline reg_t read_frs3(hwacha_t* h, insn_t insn, uint32_t idx)
{
  if (INSN_RS3 >= h->get_ct_state()->nfpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  return UT_FRS3(idx);
}

static inline void write_frd(hwacha_t* h, insn_t insn, uint32_t idx, reg_t value)
{
  if (INSN_RD >= h->get_ct_state()->nfpr)
    h->take_exception(HWACHA_CAUSE_VF_ILLEGAL_REGID, VF_PC);
  UT_WRITE_FRD(idx, value);
}

#define FRS1 read_frs1(h, insn, UTIDX)
#define FRS2 read_frs2(h, insn, UTIDX)
#define FRS3 read_frs3(h, insn, UTIDX)
#define WRITE_FRD(value) write_frd(h, insn, UTIDX, value)

// we assume the vector unit has floating-point alus
#undef require_fp
#define require_fp

#include "cvt16.h"

#define HFRS1 cvt_hs(FRS1)
#define HFRS2 cvt_hs(FRS2)
#define HFRS3 cvt_hs(FRS3)

#define WRITE_HFRD(value) write_frd(h, insn, UTIDX, cvt_sh(value, RM))

#define sext16(x) ((sreg_t)(int16_t)(x))

#endif
