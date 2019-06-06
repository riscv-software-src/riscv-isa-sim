// vflt.vf vd, vs2, rs1
VI_VFP_LOOP_CMP
({
  res = f32_lt_quiet(vs2, rs1);
})
