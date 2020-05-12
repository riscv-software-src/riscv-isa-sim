// vfredosum: vd[0] =  sum( vs2[*] , vs1[0] )
bool is_propagate = false;
VI_VFP_VV_LOOP_REDUCTION
({
  vd_0 = f16_add(vd_0, vs2);
},
{
  vd_0 = f32_add(vd_0, vs2);
},
{
  vd_0 = f64_add(vd_0, vs2);
})
