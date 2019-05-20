// vfredsum: vd[0] =  sum( vs2[*] , vs1[0] )
VFP_VV_LOOP_REDUCTION
({
  vd_0 = f32_add(vd_0, vs2);
})
VI_CHECK_1905
