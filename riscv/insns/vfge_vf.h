// vfge.vf vd, vs2, rs1
VFP_LOOP_CMP
({
  res = !f32_lt(vs2, rs1);
})
