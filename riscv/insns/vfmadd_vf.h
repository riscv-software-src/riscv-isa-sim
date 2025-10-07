// vfmadd: vd[i] = +(vd[i] * f[rs1]) + vs2[i]
require_zvfbfa

VI_VFP_VF_LOOP
({
  vd = VFP_MULADD_16(vd, rs1, vs2);
},
{
  vd = f32_mulAdd(vd, rs1, vs2);
},
{
  vd = f64_mulAdd(vd, rs1, vs2);
})
