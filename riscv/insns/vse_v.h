// vsw.v and vsseg[2-8]w.v
const reg_t sew = p->VU.vsew;
const reg_t nf = insn.v_nf() + 1;
const reg_t vl = p->VU.vl;
const reg_t elt_byte = sew / 8;
require(sew >= e8 && sew <= e64);
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t baseAddr = RS1;
reg_t vs3 = insn.rd();
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)

  if (!is_valid)
    continue;

  for (reg_t fn = 0; fn < nf; ++fn) {
    switch (sew) {
    case e8:
      MMU.store_uint8(baseAddr + (i * nf + fn) * elt_byte,
          P.VU.elt<uint8_t>(vs3 + fn, vreg_inx));
      break;
    case e16:
      MMU.store_uint16(baseAddr + (i * nf + fn) * elt_byte,
          P.VU.elt<uint16_t>(vs3 + fn, vreg_inx));
      break;
    case e32:
      MMU.store_uint32(baseAddr + (i * nf + fn) * elt_byte,
          P.VU.elt<uint32_t>(vs3 + fn, vreg_inx));
      break;
    case e64:
      MMU.store_uint64(baseAddr + (i * nf + fn) * elt_byte,
          P.VU.elt<uint64_t>(vs3 + fn, vreg_inx));
      break;
    }
  }
}
p->VU.vstart = 0;
VI_CHECK_1905
