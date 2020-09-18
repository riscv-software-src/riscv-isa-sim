// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd.v = f16_classify(vs2);
},
{
  vd.v = f32_classify(vs2);
},
{
  vd.v = f64_classify(vs2);
})
