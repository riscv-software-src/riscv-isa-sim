// vfwmaccbf16.vf vd, vs2, rs1
VI_VFP_BF16_VF_LOOP_WIDE
({
  vd = f32_mulAdd(rs1, vs2, vd);
})
