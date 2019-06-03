// vfnmadd: vd[i] = -(vd[i] * f[rs1]) - vs2[i]
VFP_VF_LOOP
({
  switch(P.VU.vsew){
  case e32:
    vd = f32_mulAdd(f32(vd.v ^ F32_SIGN), rs1, f32(vs2.v ^ F32_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
