// vfmv_vf vd, vs1
require((insn.rd() & (P.VU.vlmul - 1)) == 0);
VI_VFP_COMMON
reg_t sew = P.VU.vsew;
for (reg_t i=P.VU.vstart; i<vl; ++i) {
  auto &vd = P.VU.elt<float32_t>(rd_num, i);
  auto rs1 = f32(READ_FREG(rs1_num));

  vd = rs1;
}

P.VU.vstart = 0;
