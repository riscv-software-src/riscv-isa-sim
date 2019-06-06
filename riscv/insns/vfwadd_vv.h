// vfwadd.vv vd, vs2, vs1
VI_VFP_VV_LOOP_WIDE
({
  vd = f64_add(vs2, vs1);
})
