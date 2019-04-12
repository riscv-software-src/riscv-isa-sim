// vsrl  vd, vs2, vs1
VI_VV_ULOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = vs2 >> (vs1 & ((1u << log2(sew)) - 1));
})
