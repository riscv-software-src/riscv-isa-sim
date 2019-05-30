// vlbu.v and vlseg[2-8]bu.v
require(p->VU.vsew >= e8);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t vd = insn.rd();
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)
  for (reg_t fn = 0; fn < nf; ++fn) {
    uint64_t val = MMU.load_uint8(baseAddr + (i *nf + fn) * 1);
    if (p->VU.vsew == e8) {
      p->VU.elt<uint8_t>(vd + fn, vreg_inx) = is_valid ? val : 0;
    } else if (p->VU.vsew == e16) {
      p->VU.elt<uint16_t>(vd + fn, vreg_inx) = is_valid ? val : 0;
    } else if (p->VU.vsew == e32) {
      p->VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ? val : 0;
    } else {
      p->VU.elt<uint64_t>(vd + fn, vreg_inx) = is_valid ? val : 0;
    }
  }
}

p->VU.vstart = 0;
VI_CHECK_1905
