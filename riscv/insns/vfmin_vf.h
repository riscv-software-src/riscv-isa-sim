// vfmin vd, vs2, rs1
VI_VFP_VF_LOOP
({
  vd = f16_min(vs2, rs1);
},
{
  vd = f32_min(vs2, rs1);
},
{
  vd = f64_min(vs2, rs1);
})
