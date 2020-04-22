// vfwmacc.vf vd, vs2, rs1
VI_VFP_VF_LOOP_WIDE
({
  vd = f32_mulAdd(rs1, vs2, vd);
},
{
  vd = f64_mulAdd(rs1, vs2, vd);
})
