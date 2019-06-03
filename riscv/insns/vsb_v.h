// vsb.v and vsseg[2-8]b.v
require(P.VU.vsew >= e8);
reg_t nf = insn.v_nf() + 1;
require((nf * P.VU.vlmul) <= (NVPR / 4));
reg_t vl = P.VU.vl;
reg_t baseAddr = RS1;
reg_t vs3 = insn.rd();
reg_t vlmax = P.VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  STRIP(i)
  V_ELEMENT_SKIP(i);

  if (!is_valid)
    continue;

  for (reg_t fn = 0; fn < nf; ++fn) {
    uint8_t val = 0;
    switch (P.VU.vsew) {
    case e8:
      val = P.VU.elt<uint8_t>(vs3 + fn, vreg_inx);
      break;
    case e16:
      val = P.VU.elt<uint16_t>(vs3 + fn, vreg_inx);
      break;
    case e32:
      val = P.VU.elt<uint32_t>(vs3 + fn, vreg_inx);
      break;
    default:
      val = P.VU.elt<uint64_t>(vs3 + fn, vreg_inx);
      break;
    }

    MMU.store_uint8(baseAddr + (i * nf + fn) * 1, val);
  }
}
P.VU.vstart = 0;
VI_CHECK_1905
