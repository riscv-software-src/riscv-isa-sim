// vmfge.vf vd, vs2, rs1
VI_VFP_VF_LOOP_CMP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  res = P.VU.altfmt ? bf16_le(rs1, vs2) : f16_le(rs1, vs2);
},
{
  res = f32_le(rs1, vs2);
},
{
  res = f64_le(rs1, vs2);
})
