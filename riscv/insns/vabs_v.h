// vabs.v vd, vs2, vm

#include "v_ext_macros.h"

require_zvabd;

VI_V_LOOP
({
  vd = vs2 > 0 ? vs2 : -vs2;
})
