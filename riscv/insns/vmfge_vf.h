// vfge.vf vd, vs2, rs1
VFP_LOOP_CMP
({
  res = f32_le_quiet(rs1, vs2);
})
