// vfmerge_vf vd, vs2, vs1, vm
VI_CHECK_SSS(false);
VI_VFP_COMMON;
reg_t sew = P.VU.vsew;
for (reg_t i=P.VU.vstart; i<vl; ++i) {
  auto &vd = P.VU.elt<float32_t>(rd_num, i);
  auto rs1 = f32(READ_FREG(rs1_num));
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);

  int midx = (P.VU.vmlen * i) / 64;
  int mpos = (P.VU.vmlen * i) % 64;
  bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

  vd = use_first ? rs1 : vs2;
}

P.VU.vstart = 0;
