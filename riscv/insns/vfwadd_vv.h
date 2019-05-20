// vfwadd.vv vd, vs2, vs1
VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    P.VU.elt<float64_t>(rd_num, i) = f64_add(f32_to_f64(vs2), f32_to_f64(vs1));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
VI_CHECK_1905
