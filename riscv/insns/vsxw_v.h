// vsxw.v and vsxseg[2-8]w.v
require(p->VU.vsew >= e32);
reg_t nf = insn.v_nf() + 1;
require((nf * p->VU.vlmul) <= (NVPR / 4));
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vs3 = insn.rd();
reg_t vlmax = p->VU.vlmax;
DUPLICATE_VREG(stride, vlmax);
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)

  for (reg_t fn = 0; fn < nf; ++fn) {
    switch (p->VU.vsew) {
    case e32:
      if (is_valid)
        MMU.store_uint32(baseAddr + index[i] + fn * 4,
                        p->VU.elt<uint32_t>(vs3 + fn, vreg_inx));
      break;
    case e64:
      if (is_valid)
        MMU.store_uint32(baseAddr + index[i] + fn * 4,
                         p->VU.elt<uint64_t>(vs3 + fn, vreg_inx));
      break;
    }
  }
}
p->VU.vstart = 0;
VI_CHECK_1905
