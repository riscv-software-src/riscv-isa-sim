// vclmulh.vx vd, vs2, rs1, vm

#include "zvk_ext_macros.h"

require_any_zvbc;
require_either_extension_condition(EXT_ZVBC, P.VU.vsew == 64, EXT_ZVBC32E, P.VU.vsew == 32 || P.VU.vsew == 16 || P.VU.vsew == 8);

VI_VX_ULOOP
({
  // Perform a carryless multiplication SEW-bit x SEW-bit on each SEW-bit element,
  // return the high SEW bits of the (2.SEW)-bit product.
  //   <https://en.wikipedia.org/wiki/Carry-less_product>
  vd = 0;
  for (std::size_t bit_idx = 1; bit_idx < sew; ++bit_idx) {
    const reg_t mask = ((reg_t) 1) << bit_idx;
    if ((rs1 & mask) != 0) {
      vd ^= ((reg_t)vs2) >> (sew - bit_idx);
    }
  }
})
