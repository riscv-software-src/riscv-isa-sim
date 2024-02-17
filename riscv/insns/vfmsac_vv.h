// vfmsac: vd[i] = +(vs1[i] * vs2[i]) - vd[i]
VI_VFP_VV_LOOP
({
  vd = f8_1_mulAdd(vs1, vs2, f8_1(vd.v ^ F8_SIGN));
},
{
  vd = f8_2_mulAdd(vs1, vs2, f8_2(vd.v ^ F8_SIGN));
},
{
  vd = f16_mulAdd(vs1, vs2, f16(vd.v ^ F16_SIGN));
},
{
  vd = f32_mulAdd(vs1, vs2, f32(vd.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(vs1, vs2, f64(vd.v ^ F64_SIGN));
})
