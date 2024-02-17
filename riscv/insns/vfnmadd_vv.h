// vfnmadd: vd[i] = -(vd[i] * vs1[i]) - vs2[i]
VI_VFP_VV_LOOP
({
  vd = f8_1_mulAdd(f8_1(vd.v ^ F8_SIGN), vs1, f8_1(vs2.v ^ F8_SIGN));
},
{
  vd = f8_2_mulAdd(f8_2(vd.v ^ F8_SIGN), vs1, f8_2(vs2.v ^ F8_SIGN));
},
{
  vd = f16_mulAdd(f16(vd.v ^ F16_SIGN), vs1, f16(vs2.v ^ F16_SIGN));
},
{
  vd = f32_mulAdd(f32(vd.v ^ F32_SIGN), vs1, f32(vs2.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(f64(vd.v ^ F64_SIGN), vs1, f64(vs2.v ^ F64_SIGN));
})
