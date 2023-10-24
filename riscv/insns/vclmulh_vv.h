// vclmulh.vv vd, vs2, vs1, vm

#include "zvk_ext_macros.h"

require_any_zvbc;
require_either_extension_condition(EXT_ZVBC, P.VU.vsew == 64, EXT_ZVBC32E, P.VU.vsew == 32);

VI_VV_ULOOP
({
  // Perform a carryless multiplication 64bx64b on each 64b element,
  // return the high 64b of the 128b product.
  //   <https://en.wikipedia.org/wiki/Carry-less_product>
  vd = 0;
  for (std::size_t bit_idx = 1; bit_idx < sew; ++bit_idx) {
    const reg_t mask = ((reg_t) 1) << bit_idx;
    if ((vs1 & mask) != 0) {
      vd ^= ((reg_t)vs2) >> (sew - bit_idx);
    }
  }
})
