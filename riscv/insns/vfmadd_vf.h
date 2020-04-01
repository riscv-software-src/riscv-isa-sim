// vfmadd: vd[i] = +(vd[i] * f[rs1]) + vs2[i]
VI_VFP_VF_LOOP
({
  vd = f16_mulAdd(vd, rs1, vs2);
},
{
  vd = f32_mulAdd(vd, rs1, vs2);
},
{
  vd = f64_mulAdd(vd, rs1, vs2);
})
