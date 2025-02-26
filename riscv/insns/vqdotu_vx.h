// vqdotu.vx vd, vs2, rs1, vm
#include "vqdot_common.h"

require_extension(EXT_ZVQDOTQ);
require(P.VU.vsew == e32);

VI_VX_LOOP
({
  VQDOT(rs1, vs2, uint8_t, uint8_t);
  vd = (vd + result) & 0xffffffff;
})
