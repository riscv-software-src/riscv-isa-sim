// vfsub.vf vd, vs2, rs1
VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_sub(rs1, vs2) : f16_sub(rs1, vs2);
},
{
  vd = f32_sub(rs1, vs2);
},
{
  vd = f64_sub(rs1, vs2);
})
