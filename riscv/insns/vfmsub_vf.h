// vfmsub: vd[i] = +(vd[i] * f[rs1]) - vs2[i]
VI_VFP_VF_LOOP
({
  vd = f32_mulAdd(vd, rs1, f32(vs2.v ^ F32_SIGN));
})
