// vfredmax vd, vs2, vs1
bool is_propagate = false;
VI_VFP_VV_LOOP_REDUCTION
({
  vd_0 = f16_max(vd_0, vs2);
},
{
  vd_0 = f32_max(vd_0, vs2);
},
{
  vd_0 = f64_max(vd_0, vs2);
})
