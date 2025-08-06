// vfmacc.vf vd, rs1, vs2, vm    # vd[i] = +(vs2[i] * x[rs1]) + vd[i]
VI_VFP_VF_LOOP
({
  rs1 = P.VU.altfmt ? READ_FREG_BF(rs1_num) : rs1;
  vd = P.VU.altfmt ? bf16_mulAdd(rs1, vs2, vd) : f16_mulAdd(rs1, vs2, vd);
},
{
  vd = f32_mulAdd(rs1, vs2, vd);
},
{
  vd = f64_mulAdd(rs1, vs2, vd);
})
