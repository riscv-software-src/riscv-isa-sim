// vfnmacc: vd[i] = -(vs1[i] * vs2[i]) - vd[i]
VI_VFP_VV_LOOP
({
  vd = f32_mulAdd(f32(vs2.v ^ F32_SIGN), vs1, f32(vd.v ^ F32_SIGN));
})
