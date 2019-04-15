// vmacc.vv: vd[i] = +(vs1[i] * vs2[i]) + vd[i]
VI_VV_LOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = vs1 * vs2 + vd;
})
