// vnmsub.vv: vd[i] = -(vd[i] * vs1[i]) + vs2[i]
VI_VV_LOOP
({
  vd = -(vd * vs1) + vs2;
})
