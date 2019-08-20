// vfncvt.f.xu.v vd, vs2, vm
VI_VFP_LOOP_BASE
  VI_CHECK_SD;
  auto vs2 = P.VU.elt<uint64_t>(rs2_num, i);
  P.VU.elt<float32_t>(rd_num, i) = ui64_to_f32(vs2);
VI_VFP_LOOP_END
