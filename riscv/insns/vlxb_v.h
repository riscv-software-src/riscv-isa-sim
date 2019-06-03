// vlxb.v and vlsseg[2-8]b.v
require(P.VU.vsew >= e8);
reg_t nf = insn.v_nf() + 1;
require((nf * P.VU.vlmul) <= (NVPR / 4));
reg_t vl = P.VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vd = insn.rd();
reg_t vlmax = P.VU.vlmax;
DUPLICATE_VREG(stride, vlmax);
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)

  for (reg_t fn = 0; fn < nf; ++fn) {
    switch (P.VU.vsew) {
      case e8:
        P.VU.elt<uint8_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index[i] + fn * 1) : 0;
        break;
      case e16:
        P.VU.elt<uint16_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index[i] + fn * 1) : 0;
        break;
      case e32:
        P.VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index[i] + fn * 1) : 0;
        break;
      case e64:
        P.VU.elt<uint64_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index[i] + fn * 1) : 0;
        break;
    }
  }
}

P.VU.vstart = 0;
VI_CHECK_1905
