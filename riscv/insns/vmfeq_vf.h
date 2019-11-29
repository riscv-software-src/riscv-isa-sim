// vmfeq.vf vd, vs2, fs1
VI_VFP_LOOP_CMP
({
  res = f32_eq(vs2, rs1);
},
{
  res = f64_eq(vs2, rs1);
},
false)
