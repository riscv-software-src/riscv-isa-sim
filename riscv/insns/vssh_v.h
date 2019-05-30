// vssh.v and vssseg[2-8]h.v
require(p->VU.vsew >= e16);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vs3 = insn.rd();
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  STRIP(i)
  V_ELEMENT_SKIP(i);

  for (reg_t fn = 0; fn < nf; ++fn) {
    uint16_t val = 0;
    switch (p->VU.vsew) {
    case e16:
      val = p->VU.elt<uint16_t>(vs3 + fn, vreg_inx);
      break;
    case e32:
      val = p->VU.elt<uint32_t>(vs3 + fn, vreg_inx);
      break;
    default:
      val = p->VU.elt<uint64_t>(vs3 + fn, vreg_inx);
      break;
    }
    if (is_valid)
      MMU.store_uint16(baseAddr + i * stride + fn * 2, val);
  }
}
p->VU.vstart = 0;
VI_CHECK_1905
