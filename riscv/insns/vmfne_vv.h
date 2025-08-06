// vmfne.vv vd, vs2, rs1
VI_VFP_VV_LOOP_CMP
({
  res = P.VU.altfmt() ? !bf16_eq(vs2, vs1) : !f16_eq(vs2, vs1);
},
{
  res = !f32_eq(vs2, vs1);
},
{
  res = !f64_eq(vs2, vs1);
})
