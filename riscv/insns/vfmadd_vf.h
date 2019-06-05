// vfmadd: vd[i] = +(vd[i] * f[rs1]) + vs2[i]
VI_VFP_VF_LOOP
({
  switch(P.VU.vsew){
  case e32:
    vd = f32_mulAdd(vd, rs1, vs2);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
