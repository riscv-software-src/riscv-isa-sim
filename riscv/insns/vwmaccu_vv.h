// vwmaccu.vv vd, vs2, vs1
VI_WIDE_CHECK_TWO;
VI_VV_LOOP_WIDEN
({
  V_WIDE_OP_AND_ASSIGN(vs2, vs1, vd_w, *, +, uint);
})
VI_CHECK_1905
