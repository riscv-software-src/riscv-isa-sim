// vssb.v and vssseg[2-8]b.v
require(p->VU.vsew >= e8);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vs3 = insn.rd();
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  for (reg_t fn = 0; fn < nf; ++fn) {
    uint8_t val = 0;
    switch (p->VU.vsew) {
    case e8:
      val = p->VU.elt<uint8_t>(vs3 + fn, i);
      break;
    case e16:
      val = p->VU.elt<uint16_t>(vs3 + fn, i);
      break;
    case e32:
      val = p->VU.elt<uint32_t>(vs3 + fn, i);
      break;
    default:
      val = p->VU.elt<uint64_t>(vs3 + fn, i);
      break;
    }
    STRIP(i * nf + fn)
    V_ELEMENT_SKIP(mmu_inx);

    MMU.store_uint8(baseAddr + mmu_inx * 1, val);
  }
}
p->VU.vstart = 0;
VI_CHECK_1905
