// vpopc.v vd, vs2

#include "zvk_ext_macros.h"

require_zvbb;

VI_V_ULOOP
({
  reg_t count = 0;
  for (std::size_t i = 0; i < P.VU.vsew; ++i) {
    if (1 & (vs2 >> i)) {
      count++;
    }
  }
  vd = count;
})
