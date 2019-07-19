require_vector;
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
const reg_t nf = insn.v_nf() + 1;
require((nf * P.VU.vlmul) <= (NVPR / 4));
const reg_t sew = P.VU.vsew;
const reg_t vl = P.VU.vl;
const reg_t baseAddr = RS1;
const reg_t rd_num = insn.rd();
bool early_stop = false;
const reg_t vlmul = P.VU.vlmul;
for (reg_t i = 0; i < P.VU.vlmax && vl != 0; ++i) {
  bool is_valid = true;
  bool is_zero = false;
  VI_STRIP(i);
  VI_ELEMENT_SKIP(i);

  for (reg_t fn = 0; fn < nf; ++fn) {
    MMU.load_uint8(baseAddr + (i * nf + fn) * 1);

    switch (sew) {
    case e8:
      P.VU.elt<uint8_t>(rd_num + fn * vlmul, vreg_inx) =
        is_valid ? MMU.load_uint8(baseAddr + (i * nf + fn) * 1) : 0;
      is_zero = is_valid && P.VU.elt<uint8_t>(rd_num + fn * vlmul, vreg_inx) == 0;
      break;
    case e16:
      P.VU.elt<uint16_t>(rd_num + fn * vlmul, vreg_inx) =
        is_valid ? MMU.load_uint16(baseAddr + (i * nf + fn) * 2) : 0;
      is_zero = is_valid && P.VU.elt<uint16_t>(rd_num + fn * vlmul, vreg_inx) == 0;
      break;
    case e32:
      P.VU.elt<uint32_t>(rd_num + fn * vlmul, vreg_inx) =
        is_valid ? MMU.load_uint32(baseAddr + (i * nf + fn) * 4) : 0;
      is_zero = is_valid && P.VU.elt<uint32_t>(rd_num + fn * vlmul, vreg_inx) == 0;
      break;
    case e64:
      P.VU.elt<uint64_t>(rd_num + fn * vlmul, vreg_inx) =
        is_valid ? MMU.load_uint64(baseAddr + (i * nf + fn) * 8) : 0;
      is_zero = is_valid && P.VU.elt<uint64_t>(rd_num + fn * vlmul, vreg_inx) == 0;
      break;
    }

    if (is_zero) {
      P.VU.vl = i;
      early_stop = true;
      break;
    }
  }

  if (early_stop) {
    break;
  }
}

P.VU.vstart = 0;
