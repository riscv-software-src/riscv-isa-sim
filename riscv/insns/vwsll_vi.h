// vwsll.vi vd, vs2, zimm5, vm

#include "zvk_ext_macros.h"

require_zvbb;

VI_ZVK_VI_WIDENING_ULOOP({
  const reg_t shift = zimm5 & ((2 * sew) - 1);
  vd_w = vs2_w << shift;
});
