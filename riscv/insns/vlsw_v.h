// vlsw.v and vlsseg[2-8]w.v
require(P.VU.vsew >= e32);
reg_t nf = insn.v_nf() + 1;
require((nf * P.VU.vlmul) <= (NVPR / 4));
reg_t vl = P.VU.vl;
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vd = insn.rd();
reg_t vlmax = P.VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)
  for (reg_t fn = 0; fn < nf; ++fn) {
 
    int64_t val = MMU.load_int32(baseAddr + i * stride + fn * 4);
    if (P.VU.vsew == e32) {
      P.VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ? val : 0;
    } else {
      P.VU.elt<uint64_t>(vd + fn, vreg_inx) = is_valid ? val : 0;
    }
  }
}

P.VU.vstart = 0;
VI_CHECK_1905
