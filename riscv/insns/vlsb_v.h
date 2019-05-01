// vlsb.v and vlsseg[2-8]b.v
require(p->VU.vsew >= e8);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vd = insn.rd();
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

  for (reg_t fn = 0; fn < nf; ++fn) {
    int64_t val = MMU.load_int8(baseAddr + i * stride + fn * 1);
    if (p->VU.vsew == e8) {
      p->VU.elt<uint8_t>(vd + fn, i) = val;
    } else if (p->VU.vsew == e16) {
      p->VU.elt<uint16_t>(vd + fn, i) = val;
    } else if (p->VU.vsew == e32) {
      p->VU.elt<uint32_t>(vd + fn, i) = val;
    } else {
      p->VU.elt<uint64_t>(vd + fn, i) = val;
    }
  }
}

//zero unfilled part
for (reg_t i = vl; i < p->VU.vlmax; ++i) {
  for (reg_t fn = 0; fn < nf; ++fn) {
    if (p->VU.vsew == e8) {
      p->VU.elt<uint8_t>(vd + fn, i) = 0;
    } else if (p->VU.vsew == e16) {
      p->VU.elt<uint16_t>(vd + fn, i) = 0;
    } else if (p->VU.vsew == e32) {
      p->VU.elt<uint32_t>(vd + fn, i) = 0;
    } else {
      p->VU.elt<uint64_t>(vd + fn, i) = 0;
    }
  }
}
p->VU.vstart = 0;
