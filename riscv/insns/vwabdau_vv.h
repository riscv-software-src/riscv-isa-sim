// vwabdau.vv vd, vs2, vs1, vm

require_zvabd;
require(P.VU.vsew <= e16);
VI_CHECK_DSS(true);

VI_VV_LOOP_WIDEN
({
  VI_WIDE_OP_MACRO_AND_ASSIGN(vs2, vs1, vd_w, DO_ABD, uint);
})
