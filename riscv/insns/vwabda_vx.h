// vwabda.vx vd, vs2, rs1, vm

require_zvabd;
require(P.VU.vsew <= e16);
VI_CHECK_DSS(false);

VI_VX_LOOP_WIDEN
({
  VI_WIDE_OP_MACRO_AND_ASSIGN(vs2, rs1, vd_w, DO_ABD, int);
})
