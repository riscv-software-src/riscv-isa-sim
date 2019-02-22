// vfmul.vv vd, vs1, vs2, vm
VFP_VV_LOOP
({
  vd = f32_mul(vs1, vs2);
 })
