// vfmacc.vv vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * vs1[i]) + vd[i]
VI_VFP_VV_LOOP
({
  vd = f8_1_mulAdd(vs1, vs2, vd);
},
{
  vd = f8_2_mulAdd(vs1, vs2, vd);
},
{
  vd = f16_mulAdd(vs1, vs2, vd);
},
{
  vd = f32_mulAdd(vs1, vs2, vd);
},
{
  vd = f64_mulAdd(vs1, vs2, vd);
})
