// vnsrl.vv vd, vs2, vs1
VI_VV_LOOP_NSHIFT
({
  vd = vs2_u >> (vs1 & (sew * 2 - 1));
})
