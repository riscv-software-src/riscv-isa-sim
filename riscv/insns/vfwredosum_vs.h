// vfwredosum.vs vd, vs2, vs1
bool is_propagate = false;
VI_VFP_VV_LOOP_WIDE_REDUCTION
({
  vd_0 = f32_add(vd_0, vs2);
},
{
  vd_0 = f64_add(vd_0, vs2);
})
