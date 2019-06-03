// vsxh.v and vsxseg[2-8]h.v
require(P.VU.vsew >= e16);
reg_t nf = insn.v_nf() + 1;
require((nf * P.VU.vlmul) <= (NVPR / 4));
reg_t vl = P.VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vs3 = insn.rd();
reg_t vlmax = P.VU.vlmax;
DUPLICATE_VREG(stride, vlmax);
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)

  for (reg_t fn = 0; fn < nf; ++fn) {
    switch (P.VU.vsew) {
    case e16:
      if (is_valid)
        MMU.store_uint16(baseAddr + index[i] + fn * 2,
                         P.VU.elt<uint16_t>(vs3 + fn, vreg_inx));
      break;
    case e32:
      if (is_valid)
        MMU.store_uint16(baseAddr + index[i] + fn * 2,
                       P.VU.elt<uint32_t>(vs3 + fn, vreg_inx));
      break;
    case e64:
      if (is_valid)
        MMU.store_uint16(baseAddr + index[i] + fn * 2,
                       P.VU.elt<uint64_t>(vs3 + fn, vreg_inx));
      break;
    }
  }
}
P.VU.vstart = 0;
VI_CHECK_1905
