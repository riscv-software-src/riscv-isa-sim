// vfnmsub: vd[i] = -(vd[i] * f[rs1]) + vs2[i]
VI_VFP_VF_LOOP
({
  vd = f32_mulAdd(f32(vd.v ^ F32_SIGN), rs1, vs2);
})
