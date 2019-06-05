// vfrdiv.vf vd, vs2, rs1, vm  # scalar-vector, vd[i] = f[rs1]/vs2[i]
VI_VFP_VF_LOOP
({
  vd = f32_div(rs1, vs2);
})
