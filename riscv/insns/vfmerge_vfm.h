// vfmerge_vf vd, vs2, vs1, vm
VI_CHECK_SSS(false);
VI_VFP_COMMON;

switch(P.VU.vsew) {
  case e16:
    for (reg_t i=P.VU.vstart; i<vl; ++i) {
      auto &vd = P.VU.elt<float16_t>(rd_num, i, true);
      auto rs1 = f16(READ_FREG(rs1_num));
      auto vs2 = P.VU.elt<float16_t>(rs2_num, i);

      int midx = i / 64;
      int mpos = i % 64;
      bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

      vd = use_first ? rs1 : vs2;
    }
    break;
  case e32:
    for (reg_t i=P.VU.vstart; i<vl; ++i) {
      auto &vd = P.VU.elt<float32_t>(rd_num, i, true);
      auto rs1 = f32(READ_FREG(rs1_num));
      auto vs2 = P.VU.elt<float32_t>(rs2_num, i);

      int midx = i / 64;
      int mpos = i % 64;
      bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

      vd = use_first ? rs1 : vs2;
    }
    break;
  case e64:
    for (reg_t i=P.VU.vstart; i<vl; ++i) {
      auto &vd = P.VU.elt<float64_t>(rd_num, i, true);
      auto rs1 = f64(READ_FREG(rs1_num));
      auto vs2 = P.VU.elt<float64_t>(rs2_num, i);

      int midx = i / 64;
      int mpos = i % 64;
      bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

      vd = use_first ? rs1 : vs2;
    }
    break;
  default:
    require(0);
    break;
}

P.VU.vstart = 0;
