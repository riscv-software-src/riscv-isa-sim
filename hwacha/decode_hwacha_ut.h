#ifndef _DECODE_HWACHA_UT_H
#define _DECODE_HWACHA_UT_H

#include "decode_hwacha.h"

#define UTIDX (i)

#undef RS1
#undef RS2
#undef WRITE_RD

#define RS1 UT_RS1(UTIDX)
#define RS2 UT_RS2(UTIDX)
#define WRITE_RD(value) UT_WRITE_RD(UTIDX, value)

#undef FRS1
#undef FRS2
#undef FRS3
#undef WRITE_FRD

#define FRS1 UT_FRS1(UTIDX)
#define FRS2 UT_FRS2(UTIDX)
#define FRS3 UT_FRS3(UTIDX)
#define WRITE_FRD(value) UT_WRITE_FRD(UTIDX, value)

// we assume the vector unit has floating-point alus
#undef require_fp
#define require_fp

// YUNSUP FIXME
#undef set_fp_exceptions
#define set_fp_exceptions

#endif
