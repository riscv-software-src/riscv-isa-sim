// vfmul.vf vd, vs2, rs1, vm
VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_mul(vs2, rs1) : f16_mul(vs2, rs1);
},
{
  vd = f32_mul(vs2, rs1);
},
{
  vd = f64_mul(vs2, rs1);
})
