// vwaddu.wv vd, vs2, vs1
VI_WIDE_CHECK_DDS(true);
VI_VV_LOOP_WIDEN
({
  VI_WIDE_WVX_OP(vs1, +, uint);
})
VI_CHECK_1905
