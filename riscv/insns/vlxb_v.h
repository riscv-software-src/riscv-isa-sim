// vlxb.v and vlsseg[2-8]b.v
require(p->VU.vsew >= e8);
reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vd = insn.rd();
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)

  for (reg_t fn = 0; fn < nf; ++fn) {
    reg_t index = 0;
    switch (p->VU.vsew) {
      case e8:
        index = p->VU.elt<int8_t>(stride, vreg_inx);
        p->VU.elt<uint8_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index + fn * 1) : 0;
        break;
      case e16:
        index = p->VU.elt<int16_t>(stride, vreg_inx);
        p->VU.elt<uint16_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index + fn * 1) : 0;
        break;
      case e32:
        index = p->VU.elt<int32_t>(stride, vreg_inx);
        p->VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index + fn * 1) : 0;
        break;
      case e64:
        index = p->VU.elt<int64_t>(stride, vreg_inx);
        p->VU.elt<uint64_t>(vd + fn, vreg_inx) = is_valid ? MMU.load_int8(baseAddr + index + fn * 1) : 0;
        break;
    }
  }
}

p->VU.vstart = 0;
VI_CHECK_1905
