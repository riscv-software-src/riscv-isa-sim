#ifndef _DECODE_HWACHA_UT_HALF_H
#define _DECODE_HWACHA_UT_HALF_H

#include "decode_hwacha_ut.h"
#include "cvt16.h"

#define HFRS1 cvt_hs(FRS1)
#define HFRS2 cvt_hs(FRS2)
#define HFRS3 cvt_hs(FRS3)

#define WRITE_HFRD(value) write_frd(h, insn, UTIDX, cvt_sh(value, RM))

#define sext16(x) ((sreg_t)(int16_t)(x))

#endif
