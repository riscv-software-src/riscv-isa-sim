// vmacc.vv: vd[i] = +(vs1[i] * vs2[i]) + vd[i]
VI_VV_LOOP
({
  vd = vs1 * vs2 + vd;
})
VI_CHECK_1905
