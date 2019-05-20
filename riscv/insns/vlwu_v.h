// vlwu.v and vlseg[2-8]wu.v
require(p->VU.vsew >= e32);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t vd = insn.rd();
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

  for (reg_t fn = 0; fn < nf; ++fn) {
    uint64_t val = MMU.load_uint32(baseAddr + (i * nf + fn) * 4);
    if (p->VU.vsew == e32) {
      p->VU.elt<uint32_t>(vd + fn, i) = val;
    } else {
      p->VU.elt<uint64_t>(vd + fn, i) = val;
    }
  }
}

//zero unfilled part
if (vl != 0){
  for (reg_t i = vl; i < p->VU.vlmax; ++i) {
    for (reg_t fn = 0; fn < nf; ++fn) {
      if (p->VU.vsew == e32) {
        p->VU.elt<uint32_t>(vd + fn, i) = 0;
      } else {
        p->VU.elt<uint64_t>(vd + fn, i) = 0;
      }
    }
  }
}
p->VU.vstart = 0;
VI_CHECK_1905
