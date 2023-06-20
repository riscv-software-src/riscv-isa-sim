// vclmulh.vx vd, vs2, rs1, vm

#include "zvk_ext_macros.h"

require_zvbc;
require(P.VU.vsew == 64);

VI_VX_ULOOP
({
  // Perform a carryless multiplication 64bx64b on each 64b element,
  // return the high 64b of the 128b product.
  //   <https://en.wikipedia.org/wiki/Carry-less_product>
  vd = 0;
  for (std::size_t bit_idx = 1; bit_idx < sew; ++bit_idx) {
    const reg_t mask = ((reg_t) 1) << bit_idx;
    if ((rs1 & mask) != 0) {
      vd ^= ((reg_t)vs2) >> (sew - bit_idx);
    }
  }
})
