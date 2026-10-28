// vfnmsac.vv vd, vs1, vs2, vm   # vd[i] = -(vs2[i] * vs1[i]) + vd[i]

require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = VFP_MULADD_16(f16(vs1.v ^ F16_SIGN), vs2, vd);
},
{
  vd = f32_mulAdd(f32(vs1.v ^ F32_SIGN), vs2, vd);
},
{
  vd = f64_mulAdd(f64(vs1.v ^ F64_SIGN), vs2, vd);
})
