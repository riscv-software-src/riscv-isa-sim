// vror.vv vd, vs2, vs1, vm

#include "zvk_ext_macros.h"

require_zvbb;

// 'mask' selects the low log2(vsew) bits of the shift amount,
// to limit the maximum shift to "vsew - 1" bits.
const reg_t mask = P.VU.vsew - 1;

VI_VV_ULOOP
({
  // For .vv, the shift amount comes from the vs1 element.
  const reg_t rshift = vs1 & mask;
  const reg_t lshift = (-rshift) & mask;
  vd = (vs2 << lshift) | (vs2 >> rshift);
})
