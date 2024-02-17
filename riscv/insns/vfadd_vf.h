// vfadd.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  vd = f8_1_add(rs1, vs2);
},
{
  vd = f8_2_add(rs1, vs2);
},
{
  vd = f16_add(rs1, vs2);
},
{
  vd = f32_add(rs1, vs2);
},
{
  vd = f64_add(rs1, vs2);
})
