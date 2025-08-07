// vfmacc.vv vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * vs1[i]) + vd[i]

require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = P.VU.altfmt ? bf16_mulAdd(vs1, vs2, vd) : f16_mulAdd(vs1, vs2, vd);
},
{
  vd = f32_mulAdd(vs1, vs2, vd);
},
{
  vd = f64_mulAdd(vs1, vs2, vd);
})
