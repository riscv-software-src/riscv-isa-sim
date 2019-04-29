// vsra.vv  vd, vs2, vs1
VI_VV_LOOP
({
  vd = vs2 >> (vs1 & ((1u << log2(sew)) - 1));
})
