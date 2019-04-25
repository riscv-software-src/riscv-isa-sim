// vfnmacc: vd[i] = -(vs1[i] * vs2[i]) - vd[i]
VFP_VV_LOOP
({
  switch(STATE.VU.vsew){
  case e32:
    vd = f32_mulAdd(f32(vs2.v ^ F32_SIGN), vs1, f32(vd.v ^ F32_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
