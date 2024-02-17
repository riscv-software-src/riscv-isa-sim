// vfmacc.vf vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * x[rs1]) + vd[i]
VI_VFP_VF_LOOP
({
  vd = f8_1_mulAdd(rs1, vs2, vd);
},
{
  vd = f8_2_mulAdd(rs1, vs2, vd);
},
{
  vd = f16_mulAdd(rs1, vs2, vd);
},
{
  vd = f32_mulAdd(rs1, vs2, vd);
},
{
  vd = f64_mulAdd(rs1, vs2, vd);
})
