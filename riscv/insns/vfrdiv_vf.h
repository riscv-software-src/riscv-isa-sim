// vfrdiv.vf vd, vs2, rs1, vm  # scalar-vector, vd[i] = f[rs1]/vs2[i]
VI_VFP_VF_LOOP
({
  vd = f8_1_div(rs1, vs2);
},
{
  vd = f8_2_div(rs1, vs2);
},
{
  vd = f16_div(rs1, vs2);
},
{
  vd = f32_div(rs1, vs2);
},
{
  vd = f64_div(rs1, vs2);
})
