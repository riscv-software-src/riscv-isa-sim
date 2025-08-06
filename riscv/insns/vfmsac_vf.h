// vfmsac: vd[i] = +(f[rs1] * vs2[i]) - vd[i]
VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_mulAdd(rs1, vs2, bf16(vd.v ^ BF16_SIGN))
                   :  f16_mulAdd(rs1, vs2,  f16(vd.v ^  F16_SIGN));
},
{
  vd = f32_mulAdd(rs1, vs2, f32(vd.v ^ F32_SIGN));
},
{
  vd = f64_mulAdd(rs1, vs2, f64(vd.v ^ F64_SIGN));
})
