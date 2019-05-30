// vlxh.v and vlxseg[2-8]h.v
require(p->VU.vsew >= e16);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vd = insn.rd();
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  STRIP(i)
  V_ELEMENT_SKIP(i);

  for (reg_t fn = 0; fn < nf; ++fn) {
    reg_t index = 0;
    switch (p->VU.vsew) {
    case e16:
      index = p->VU.elt<int16_t>(stride, i);
      p->VU.elt<uint16_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int16(baseAddr + index + fn * 2) : 0;
      break;
    case e32:
      index = p->VU.elt<int32_t>(stride, i);
      p->VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int16(baseAddr + index + fn * 2) : 0;
      break;
    case e64:
      index = p->VU.elt<int64_t>(stride, i);
      p->VU.elt<uint64_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int16(baseAddr + index + fn * 2) : 0;
      break;
    }
  }
}

P.VU.vstart = 0;
VI_CHECK_1905
