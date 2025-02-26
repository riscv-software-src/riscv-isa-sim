// vqdotsu.vv vd, vs2, vs1, vm
#include "vqdot_common.h"

require_extension(EXT_ZVQDOTQ);
require(P.VU.vsew == e32);

VI_VV_LOOP
({
  VQDOT(vs1, vs2, uint8_t, int8_t);
  vd = (vd + result) & 0xffffffff;
})
