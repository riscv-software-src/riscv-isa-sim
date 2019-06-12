// vfmsac: vd[i] = +(f[rs1] * vs2[i]) - vd[i]
VI_VFP_VF_LOOP
({
  vd = f32_mulAdd(rs1, vs2, f32(vd.v ^ F32_SIGN));
})
