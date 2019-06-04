// vflt.vf vd, vs2, rs1
VFP_LOOP_CMP
({
  res = f32_lt_quiet(vs2, rs1);
})
