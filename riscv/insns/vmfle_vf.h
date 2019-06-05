// vfle.vf vd, vs2, rs1
VI_VFP_LOOP_CMP
({
  res = f32_le(vs2, rs1);
})
