// vfmsac: vd[i] = +(f[rs1] * vs2[i]) - vd[i]
VFP_VF_LOOP
({
  switch(STATE.VU.vsew){
  case e32:
    vd = f32_mulAdd(rs1, vs2, f32(vd.v ^ F32_SIGN));
    break;
  case e16:
  case e8:
  default:
    softfloat_exceptionFlags = 1;
    break;
  };
})
