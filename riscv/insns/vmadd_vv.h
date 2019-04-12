// vmadd: vd[i] = (vd[i] * vs1[i]) + vs2[i]
VI_VV_LOOP
({
  V_LOOP_ELEMENT_SKIP;

  vd = vd * vs1 + vs2;
})
