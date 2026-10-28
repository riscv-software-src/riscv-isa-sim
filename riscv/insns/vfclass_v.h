// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = f16(P.VU.altfmt ? bf16_classify(bf16(vs2)) : f16_classify(vs2));
},
{
  vd = f32(f32_classify(vs2));
},
{
  vd = f64(f64_classify(vs2));
})
