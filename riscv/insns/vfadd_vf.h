// vfadd.vf vd, vs2, rs1
require_zvfbfa_vsew16

VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_add(rs1, vs2) : f16_add(rs1, vs2);
},
{
  vd = f32_add(rs1, vs2);
},
{
  vd = f64_add(rs1, vs2);
})
