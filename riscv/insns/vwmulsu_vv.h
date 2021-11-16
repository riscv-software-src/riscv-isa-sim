// vwmulsu.vv vd, vs2, vs1
VI_CHECK_DSS(true);
VI_VV_LOOP_WIDEN
({
  VI_WIDE_OP_AND_ASSIGN_MIX(vs2, vs1, 0, *, +, uint, int, uint)
})
