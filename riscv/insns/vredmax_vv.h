// vredmax.vs vd, vs2 ,vs1
VI_VV_LOOP_REDUCTION
({
  vd_0_res = (vd_0_res >= vs2) ? vd_0_res : vs2;
})
