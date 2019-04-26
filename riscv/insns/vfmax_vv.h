// vfmax
VFP_VV_LOOP
({
  switch(STATE.VU.vsew){
  case e32:
    vd = f32_max(vs2, vs1);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
