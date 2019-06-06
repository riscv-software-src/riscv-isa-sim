// vfredosum: vd[0] =  sum( vs2[*] , vs1[0] )
VI_VFP_VV_LOOP_REDUCTION
({
  vd_0 = f32_add(vd_0, vs2);
})
