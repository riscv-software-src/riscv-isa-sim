// vfmul.vv vd, vs1, vs2, vm
VI_VFP_VV_LOOP
({
  vd = f8_1_mul(vs1, vs2);
},
{
  vd = f8_2_mul(vs1, vs2);
},
{
  vd = f16_mul(vs1, vs2);
},
{
  vd = f32_mul(vs1, vs2);
},
{
  vd = f64_mul(vs1, vs2);
})
