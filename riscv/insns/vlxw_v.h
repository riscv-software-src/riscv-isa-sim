// vlxw.v and vlxseg[2-8]w.v
require(p->VU.vsew >= e32);
reg_t nf = insn.v_nf() + 1;
require((nf * p->VU.vlmul) <= (NVPR / 4));
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vd = insn.rd();
reg_t vlmax = p->VU.vlmax;
DUPLICATE_VREG(stride, vlmax);
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)

  for (reg_t fn = 0; fn < nf; ++fn) {
    switch (p->VU.vsew) {
    case e32:
      p->VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int32(baseAddr + index[i] + fn * 4) : 0;
      break;
    case e64:
      p->VU.elt<uint64_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int32(baseAddr + index[i] + fn * 4) : 0;
      break;
    }
  }
}

p->VU.vstart = 0;
VI_CHECK_1905
