// vwsll.vv vd, vs2, zimm5, vm

#include "zvk_ext_macros.h"

require_zvbb;

VI_ZVK_VV_WIDENING_ULOOP({
  const reg_t shift = (vs1 & ((2 * sew) - 1));
  vd_w = vs2_w << shift;
});
