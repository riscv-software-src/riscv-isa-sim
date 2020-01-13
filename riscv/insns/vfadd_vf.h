// vfadd.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  vd = f32_add(rs1, vs2);
},
{
  vd = f64_add(rs1, vs2);
})
