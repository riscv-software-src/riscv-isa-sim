// vandn.vx vd, vs2, rs1, vm

#include "zvk_ext_macros.h"

require_zvkb;

VI_VX_LOOP
({
  vd = vs2 & (~rs1);
})
