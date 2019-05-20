// vwadd.wv vd, vs2, vs1
VI_WIDE_CHECK_TWO;
VI_VV_LOOP_WIDEN
({
  VI_WIDE_WVX_OP(vs1, +, int);
})
VI_CHECK_1905
