// vfmv_vf vd, vs1
require((insn.rd() & (P.VU.vlmul - 1)) == 0);
VI_VFP_COMMON
switch(P.VU.vsew) {
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
