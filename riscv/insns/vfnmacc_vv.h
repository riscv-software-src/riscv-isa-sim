// vfnmacc: vd[i] = -(vs1[i] * vs2[i]) - vd[i]

require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bf16_mulAdd(bf16(vs2.v ^ BF16_SIGN), vs1, bf16(vd.v ^ BF16_SIGN))
                   :  f16_mulAdd( f16(vs2.v ^  F16_SIGN), vs1,  f16(vd.v ^  F16_SIGN));
},
{
  vd = f32_mulAdd(f32(vs2.v ^ F32_SIGN), vs1, f32(vd.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(f64(vs2.v ^ F64_SIGN), vs1, f64(vd.v ^ F64_SIGN));
})
