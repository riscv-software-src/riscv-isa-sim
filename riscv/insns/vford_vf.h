// vford.vf vd, vs2, rs1, vm
VFP_LOOP_CMP
({
  res = !(f32_isSignalingNaN(vs2) || f32_isSignalingNaN(rs1));
})
