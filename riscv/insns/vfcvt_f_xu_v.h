// vfcvt.f.xu.v vd, vd2, vm
VI_VFP_VV_LOOP
({
  auto vs2_u = P.VU.elt<uint32_t>(rs2_num, i);
  vd = ui32_to_f32(vs2_u);
})
