// vfmul.vf vd, vs2, rs1, vm
VI_VFP_VF_LOOP
({
  vd = f16_mul(vs2, rs1);
},
{
  vd = f32_mul(vs2, rs1);
},
{
  vd = f64_mul(vs2, rs1);
})
