// vwsubu.wv vd, vs2, vs1
V_WIDE_CHECK;
VI_VV_LOOP
({
  VI_WIDE_WVX_OP(vs1, -, uint);
})
