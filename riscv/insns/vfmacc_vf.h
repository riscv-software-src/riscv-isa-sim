// vfmacc.vf vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * x[rs1]) + vd[i]
VI_VFP_VF_LOOP
({
  switch(P.VU.vsew) {
  case e32:
    vd = f32_mulAdd(rs1, vs2, vd);
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
  };
})
