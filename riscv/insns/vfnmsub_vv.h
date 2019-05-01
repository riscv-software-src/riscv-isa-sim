// vfnmsub: vd[i] = -(vd[i] * vs1[i]) + vs2[i]
VFP_VV_LOOP
({
  switch(p->VU.vsew){
  case e32:
    vd = f32_mulAdd(f32(vd.v ^ F32_SIGN), vs1, vs2);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
