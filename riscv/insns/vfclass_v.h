// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = P.VU.altfmt ? bf16(bf16_classify(vs2)) : f16(f16_classify(vs2));
},
{
  vd = f32(f32_classify(vs2));
},
{
  vd = f64(f64_classify(vs2));
})
