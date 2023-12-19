// vror.vi vd, vs2, zimm6, vm

#include "zvk_ext_macros.h"

require_zvbb;

// 'mask' selects the low log2(vsew) bits of the shift amount,
// to limit the maximum shift to "vsew - 1" bits.
const reg_t mask = P.VU.vsew - 1;

// For .vi, the shift amount comes from bits [26,19-15].
const reg_t rshift = insn.v_zimm6() & mask;
const reg_t lshift = (-rshift) & mask;

VI_V_ULOOP
({
  vd = (vs2 << lshift) | (vs2 >> rshift);
})
