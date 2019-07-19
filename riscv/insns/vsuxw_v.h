// vsxw.v and vsxseg[2-8]w.v
require_vector;
require(P.VU.vsew >= e32);
reg_t vl = P.VU.vl;
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vs3 = insn.rd();
reg_t vlmax = P.VU.vlmax;
VI_DUPLICATE_VREG(stride, vlmax);
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  VI_ELEMENT_SKIP(i);
  VI_STRIP(i)

  switch (P.VU.vsew) {
  case e32:
    if (is_valid)
      MMU.store_uint32(baseAddr + index[i],
                       P.VU.elt<uint32_t>(vs3, vreg_inx));
    break;
  case e64:
    if (is_valid)
      MMU.store_uint32(baseAddr + index[i],
                       P.VU.elt<uint64_t>(vs3, vreg_inx));
    break;
  }
}
P.VU.vstart = 0;
