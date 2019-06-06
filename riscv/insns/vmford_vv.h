// vford.vv vd, vs2, vs1, vm
VI_VFP_LOOP_CMP
({
  res = !(f32_isSignalingNaN(vs2) || f32_isSignalingNaN(vs1));
})
