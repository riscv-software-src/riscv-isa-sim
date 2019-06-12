// vfmacc.vf vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * x[rs1]) + vd[i]
VI_VFP_VF_LOOP
({
  vd = f32_mulAdd(rs1, vs2, vd);
})
