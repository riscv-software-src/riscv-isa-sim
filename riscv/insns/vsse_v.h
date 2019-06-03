// vsse.v and vssseg[2-8]e.v
const reg_t sew = P.VU.vsew;
const reg_t nf = insn.v_nf() + 1;
const reg_t vl = P.VU.vl;
const reg_t elt_byte = sew / 8;
require(sew >= e8 && sew <= e64);
require((nf * P.VU.vlmul) <= (NVPR / 4));
reg_t baseAddr = RS1;
reg_t stride = RS2;
reg_t vs3 = insn.rd();
reg_t vlmax = P.VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  V_ELEMENT_SKIP(i);
  STRIP(i)
  
  if (! is_valid)
    continue;

 for (reg_t fn = 0; fn < nf; ++fn) {
    switch (sew) {
    case e8:
      MMU.store_uint8(baseAddr + i * stride + fn * elt_byte,
          P.VU.elt<uint8_t>(vs3 + fn, vreg_inx));
      break;
    case e16:
      MMU.store_uint16(baseAddr + i * stride + fn * elt_byte,
          P.VU.elt<uint16_t>(vs3 + fn, vreg_inx));
      break;
    case e32:
      MMU.store_uint32(baseAddr + i * stride + fn * elt_byte,
          P.VU.elt<uint32_t>(vs3 + fn, vreg_inx));
      break;
    case e64:
      MMU.store_uint64(baseAddr + i * stride + fn * elt_byte,
          P.VU.elt<uint64_t>(vs3 + fn, vreg_inx));
      break;
    }
  }
}
P.VU.vstart = 0;
VI_CHECK_1905
