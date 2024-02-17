// vfclass.v vd, vs2, vm
VI_VFP_V_LOOP
({
  vd = f8_1(f8_1_classify(vs2));
},
{
  vd = f8_2(f8_2_classify(vs2));
},
{
  vd = f16(f16_classify(vs2));
},
{
  vd = f32(f32_classify(vs2));
},
{
  vd = f64(f64_classify(vs2));
})
