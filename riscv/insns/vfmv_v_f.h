// vfmv_vf vd, vs1
require_align(insn.rd(), P.VU.vflmul);
VI_VFP_COMMON
switch(P.VU.vsew) {
  case e16:
    for (reg_t i=P.VU.vstart; i<vl; ++i) {
      auto &vd = P.VU.elt<float16_t>(rd_num, i, true);
      auto rs1 = f16(READ_FREG(rs1_num));

      vd = rs1;
    }
    break;
  case e32:
    for (reg_t i=P.VU.vstart; i<vl; ++i) {
      auto &vd = P.VU.elt<float32_t>(rd_num, i, true);
      auto rs1 = f32(READ_FREG(rs1_num));

      vd = rs1;
    }
    break;
  case e64:
    for (reg_t i=P.VU.vstart; i<vl; ++i) {
      auto &vd = P.VU.elt<float64_t>(rd_num, i, true);
      auto rs1 = f64(READ_FREG(rs1_num));

      vd = rs1;
    }
    break;
}

P.VU.vstart = 0;
