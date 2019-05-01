// vfmsac: vd[i] = +(vs1[i] * vs2[i]) - vd[i]
VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    vd = f32_mulAdd(vs1, vs2, f32(vd.v ^ F32_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
