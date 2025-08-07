// vfmsub: vd[i] = +(vd[i] * vs1[i]) - vs2[i]

require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bf16_mulAdd(vd, vs1, bf16(vs2.v ^ BF16_SIGN))
                   :  f16_mulAdd(vd, vs1,  f16(vs2.v ^  F16_SIGN));
},
{
  vd = f32_mulAdd(vd, vs1, f32(vs2.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(vd, vs1, f64(vs2.v ^ F64_SIGN));
})
