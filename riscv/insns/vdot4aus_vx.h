// vdot4aus.vx vd, vs2, rs1, vm
#include "vdot4a_common.h"

require_extension(EXT_ZVDOT4A8I);
require(P.VU.vsew == e32);

VI_VX_LOOP({
  VDOT4A(rs1, vs2, int8_t, uint8_t);
  vd = (vd + result) & 0xffffffff;
})
