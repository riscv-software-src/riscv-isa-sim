// vdot4a.vv vd, vs2, vs1, vm
#include "vdot4a_common.h"

require_extension(EXT_ZVDOT4A8I);
require(P.VU.vsew == e32);

VI_VV_LOOP({
  VDOT4A(vs1, vs2, int8_t, int8_t);
  vd = (vd + result) & 0xffffffff;
})
