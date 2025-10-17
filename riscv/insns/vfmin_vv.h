// vfmin vd, vs2, vs1

require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = VFP_OP_16(min, vs2, vs1);
},
{
  vd = f32_min(vs2, vs1);
},
{
  vd = f64_min(vs2, vs1);
})
