// vfcvt.f.x.v vd, vd2, vm
VI_VFP_VV_LOOP
({
  auto vs2_i = P.VU.elt<int32_t>(rs2_num, i);
  vd = i32_to_f32(vs2_i);
})
