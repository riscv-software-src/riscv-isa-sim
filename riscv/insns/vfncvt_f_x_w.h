// vfncvt.f.x.v vd, vs2, vm
VI_CHECK_SD;
VI_VFP_LOOP_BASE
  auto vs2 = P.VU.elt<int64_t>(rs2_num, i);
  P.VU.elt<float32_t>(rd_num, i) = i64_to_f32(vs2);
VI_VFP_LOOP_END
