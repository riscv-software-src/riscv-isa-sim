// vsrl.vv  vd, vs2, vs1
VI_VV_ULOOP
({
  vd = vs2 >> (vs1 & ((1u << log2(sew)) - 1));
})
VI_CHECK_1905
