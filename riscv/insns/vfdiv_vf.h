// vfdiv.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  vd = f8_1_div(vs2, rs1);
},
{
  vd = f8_2_div(vs2, rs1);
},
{
  vd = f16_div(vs2, rs1);
},
{
  vd = f32_div(vs2, rs1);
},
{
  vd = f64_div(vs2, rs1);
})
