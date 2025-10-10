// vmflt.vf vd, vs2, rs1
require_zvfbfa

VI_VFP_VF_LOOP_CMP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  res = P.VU.altfmt ? bf16_lt(vs2, rs1) : f16_lt(vs2, rs1);
},
{
  res = f32_lt(vs2, rs1);
},
{
  res = f64_lt(vs2, rs1);
})
