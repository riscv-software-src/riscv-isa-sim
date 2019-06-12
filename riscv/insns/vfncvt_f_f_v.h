// VFUNARY0 encoding space
VI_VFP_LOOP_BASE
  VI_CHECK_SDS;
  auto vs2 = P.VU.elt<float64_t>(rs2_num, i);
  P.VU.elt<float32_t>(rd_num, i) = f64_to_f32(vs2);
VI_VFP_LOOP_END
VI_CHECK_1905
