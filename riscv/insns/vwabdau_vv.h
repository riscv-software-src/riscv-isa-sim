// vwabdau.vv vd, vs2, vs1

#include "zvabd_ext_macros.h"

require_zvabd;
VI_CHECK_DSS(true);

VI_VV_LOOP_WIDEN
({
  VI_WIDE_OP_MACRO_AND_ASSIGN(vs2, vs1, vd_w, DO_ABD, uint);
})

