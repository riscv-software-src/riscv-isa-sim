// vfwmul.vv vd, vs2, vs1
VFP_VVF_LOOP_WIDE
({
  switch(p->VU.vsew){
  case e32:
    vd = f64_add(vs2, vs1);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
