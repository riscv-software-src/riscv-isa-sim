// vfnmacc: vd[i] = -(f[rs1] * vs2[i]) - vd[i]
VI_VFP_VF_LOOP
({
  vd = f16_mulAdd(rs1, f16(vs2.v ^ F16_SIGN), f16(vd.v ^ F16_SIGN));
},
{
  vd = f32_mulAdd(rs1, f32(vs2.v ^ F32_SIGN), f32(vd.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(rs1, f64(vs2.v ^ F64_SIGN), f64(vd.v ^ F64_SIGN));
})
