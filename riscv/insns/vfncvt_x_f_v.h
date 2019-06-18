// vfncvt.x.f.v vd, vs2, vm
VI_VFP_LOOP_BASE
  VI_CHECK_SD;
  auto vs2 = P.VU.elt<float64_t>(rs2_num, i);
  P.VU.elt<int32_t>(rd_num, i) = f64_to_i32(vs2, STATE.frm, true);
VI_VFP_LOOP_END
