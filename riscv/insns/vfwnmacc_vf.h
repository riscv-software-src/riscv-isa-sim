// vfwnmacc.vf vd, vs2, rs1
VI_VFP_VF_LOOP_WIDE
({
  vd = f32_mulAdd(f32(rs1.v ^ F32_SIGN), vs2, f32(vd.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(f64(rs1.v ^ F64_SIGN), vs2, f64(vd.v ^ F64_SIGN));
})
