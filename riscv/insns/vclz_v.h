// vclz.v vd, vs2

#include "zvk_ext_macros.h"

require_zvbb;

VI_V_ULOOP
({
  unsigned int i = 0;
  for (; i < P.VU.vsew; ++i) {
    if (1 & (vs2 >> (P.VU.vsew - 1 - i))) {
      break;
    }
  }
  vd = i;
})
