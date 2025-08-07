// vfmsub: vd[i] = +(vd[i] * f[rs1]) - vs2[i]

require_zvfbfa

VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_mulAdd(vd, rs1, bf16(vs2.v ^ BF16_SIGN))
                   :  f16_mulAdd(vd, rs1,  f16(vs2.v ^  F16_SIGN));
},
{
  vd = f32_mulAdd(vd, rs1, f32(vs2.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(vd, rs1, f64(vs2.v ^ F64_SIGN));
})
