// vwsll.vx vd, vs2, zimm5, vm

#include "zvk_ext_macros.h"

require_zvbb;

VI_ZVK_VX_WIDENING_ULOOP({
  const reg_t shift = (rs1 & ((2 * sew) - 1));
  vd_w = vs2_w << shift;
});
