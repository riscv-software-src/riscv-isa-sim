// vfmsub: vd[i] = +(vd[i] * vs1[i]) - vs2[i]
VI_VFP_VV_LOOP
({
  vd = f32_mulAdd(vd, vs1, f32(vs2.v ^ F32_SIGN));
})
