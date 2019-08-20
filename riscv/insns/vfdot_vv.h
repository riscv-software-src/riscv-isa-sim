// vfdot.vv vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = f32_add(vd, f32_mul(vs2, vs1));
})
