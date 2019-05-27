// vnsra.vv vd, vs2, vs1
VI_VV_LOOP_NSHIFT
({
  vd = vs2 >> (vs1 & ((1u << log2(sew * 2)) - 1));
})
VI_CHECK_1905
