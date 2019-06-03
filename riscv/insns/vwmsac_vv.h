// vwmsac.vv vd, vs2, vs1
VI_CHECK_DSS(true);
VI_VV_LOOP_WIDEN
({
  V_WIDE_OP_AND_ASSIGN(vs2, vs1, vd_w, *, -, int);
})
VI_CHECK_1905
