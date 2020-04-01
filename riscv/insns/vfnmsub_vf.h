// vfnmsub: vd[i] = -(vd[i] * f[rs1]) + vs2[i]
VI_VFP_VF_LOOP
({
  vd = f16_mulAdd(f16(vd.v ^ F16_SIGN), rs1, vs2);
},
{
  vd = f32_mulAdd(f32(vd.v ^ F32_SIGN), rs1, vs2);
},
{
  vd = f64_mulAdd(f64(vd.v ^ F64_SIGN), rs1, vs2);
})
