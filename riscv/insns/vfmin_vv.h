// vfmin vd, vs2, vs1
VI_VFP_VV_LOOP
({
  vd = f32_min(vs2, vs1);
})
