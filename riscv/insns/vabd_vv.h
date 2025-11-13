// vadb.vv vd, vs1, vs2, vm

#include "zvabd_ext_macros.h"

require_zvabd;

VI_VV_LOOP
({
  vd = DO_ABD(vs1, vs2);
})
