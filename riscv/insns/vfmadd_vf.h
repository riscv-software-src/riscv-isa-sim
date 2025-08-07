// vfmadd: vd[i] = +(vd[i] * f[rs1]) + vs2[i]
require_zvfbfa

VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_mulAdd(vd, rs1, vs2) : f16_mulAdd(vd, rs1, vs2);
},
{
  vd = f32_mulAdd(vd, rs1, vs2);
},
{
  vd = f64_mulAdd(vd, rs1, vs2);
})
