// vfmsac: vd[i] = +(vs1[i] * vs2[i]) - vd[i]
VI_VFP_VV_LOOP
({
  vd = f32_mulAdd(vs1, vs2, f32(vd.v ^ F32_SIGN));
})
