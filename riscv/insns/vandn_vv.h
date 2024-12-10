// vandn.vv vd, vs2, vs1, vm

#include "zvk_ext_macros.h"

require_zvkb;

VI_VV_LOOP
({
  vd = vs2 & (~vs1);
})
