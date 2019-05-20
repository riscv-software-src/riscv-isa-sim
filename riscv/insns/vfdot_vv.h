// vfdot.vv vd, vs2, vs1
VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    vd = f32_add(vd, f32_mul(vs2, vs1));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
VI_CHECK_1905
