// vfmul.vf vd, vs2, rs1, vm
VI_VFP_VF_LOOP
({
  vd = f8_1_mul(vs2, rs1);
},
{
  vd = f8_2_mul(vs2, rs1);
},
{
  vd = f16_mul(vs2, rs1);
},
{
  vd = f32_mul(vs2, rs1);
},
{
  vd = f64_mul(vs2, rs1);
})
