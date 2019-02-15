// vfmacc.vf vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * x[rs1]) + vd[i]
VFMA_VF_LOOP
({
  rd = f32_mulAdd(rs1, rs2, rd);
 })
