// vfmadd: vd[i] = +(vd[i] * vs1[i]) + vs2[i]
require_zvfbfa

VI_VFP_VV_LOOP
({
  vd = VFP_MULADD_16(vd, vs1, vs2);
},
{
  vd = f32_mulAdd(vd, vs1, vs2);
},
{
  vd = f64_mulAdd(vd, vs1, vs2);
})
