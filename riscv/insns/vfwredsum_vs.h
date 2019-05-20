// vfwredsum.vs vd, vs2, vs1
VFP_VV_LOOP_WIDE_REDUCTION
({
  vd_0 = f64_add(vd_0, vs2);
})
VI_CHECK_1905
