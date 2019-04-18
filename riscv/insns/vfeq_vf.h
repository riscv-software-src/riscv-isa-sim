// vfeq.vf vd, vs2, fs1
VFP_LOOP_CMP
({
  res = f32_eq(vs2, rs1);
})
