// vsxe.v and vsxseg[2-8]e.v
const reg_t sew = p->VU.vsew;
const reg_t nf = insn.v_nf() + 1;
const reg_t vl = p->VU.vl;
const reg_t elt_byte = sew / 8;
require(sew >= e8 && sew <= e64);
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t baseAddr = RS1;
reg_t stride = insn.rs2();
reg_t vs3 = insn.rd();
for (reg_t i = p->VU.vstart; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

  for (reg_t fn = 0; fn < nf; ++fn) {
    reg_t index = 0;
    switch (sew) {
    case e8:
      index = p->VU.elt<int8_t>(stride, i);
      MMU.store_uint8(baseAddr + index + fn * elt_byte,
                      p->VU.elt<uint8_t>(vs3 + fn, i));
      break;
    case e16:
      index = p->VU.elt<int16_t>(stride, i);
      MMU.store_uint16(baseAddr + index + fn * elt_byte,
                       p->VU.elt<uint16_t>(vs3 + fn, i));
      break;
    case e32:
      index = p->VU.elt<int32_t>(stride, i);
      MMU.store_uint32(baseAddr + index + fn * elt_byte,
                       p->VU.elt<uint32_t>(vs3 + fn, i));
      break;
    case e64:
      index = p->VU.elt<int64_t>(stride, i);
      MMU.store_uint64(baseAddr + index + fn * elt_byte,
                       p->VU.elt<uint64_t>(vs3 + fn, i));
      break;
    }
  }
}
p->VU.vstart = 0;
