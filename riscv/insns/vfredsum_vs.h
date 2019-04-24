// vfredsum: vd[0] =  sum( vs2[*] , vs1[0] )
VFP_VV_REDUCTION_LOOP
({
  vd_0 = f32_add(vd_0, vs2);
})
