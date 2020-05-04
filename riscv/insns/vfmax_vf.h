// vfmax
VI_VFP_VF_LOOP
({
  vd = f16_max(vs2, rs1);
},
{
  vd = f32_max(vs2, rs1);
},
{
  vd = f64_max(vs2, rs1);
})
