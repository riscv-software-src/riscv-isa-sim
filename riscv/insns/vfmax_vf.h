// vfmax
VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_max(vs2, rs1) : f16_max(vs2, rs1);
},
{
  vd = f32_max(vs2, rs1);
},
{
  vd = f64_max(vs2, rs1);
})
