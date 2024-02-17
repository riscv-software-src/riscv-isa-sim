// vfsub.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  vd = f8_1_sub(rs1, vs2);
},
{
  vd = f8_2_sub(rs1, vs2);
},
{
  vd = f16_sub(rs1, vs2);
},
{
  vd = f32_sub(rs1, vs2);
},
{
  vd = f64_sub(rs1, vs2);
})
