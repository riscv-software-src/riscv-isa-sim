require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
const reg_t nf = insn.v_nf() + 1;
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
const reg_t sew = p->VU.vsew;
const reg_t vl = p->VU.vl;
const reg_t baseAddr = RS1;
const reg_t rd_num = insn.rd();
bool early_stop = false;
for (reg_t i = 0; i < P.VU.vlmax && vl != 0; ++i) {
  bool is_valid = true;
  bool is_zero = false;
  STRIP(i);
  V_ELEMENT_SKIP(i);

  for (reg_t fn = 0; fn < nf; ++fn) {
    MMU.load_uint8(baseAddr + (i * nf + fn) * 1);

    switch (sew) {
    case e8:
      p->VU.elt<uint8_t>(rd_num + fn, vreg_inx) =
          is_valid ? MMU.load_uint8(baseAddr + (i * nf + fn) * 1) : 0;
      is_zero = is_valid && p->VU.elt<uint8_t>(rd_num + fn, vreg_inx) == 0;
          break;
    case e16:
      p->VU.elt<uint16_t>(rd_num + fn, vreg_inx) =
          is_valid ? MMU.load_uint16(baseAddr + (i * nf + fn) * 2) : 0;
      is_zero = is_valid && p->VU.elt<uint16_t>(rd_num + fn, vreg_inx) == 0;
      break;
    case e32:
      p->VU.elt<uint32_t>(rd_num + fn, vreg_inx) =
          is_valid ? MMU.load_uint32(baseAddr + (i * nf + fn) * 4) : 0;
      is_zero = is_valid && p->VU.elt<uint32_t>(rd_num + fn, vreg_inx) == 0;
      break;
    case e64:
      p->VU.elt<uint64_t>(rd_num + fn, vreg_inx) =
          is_valid ? MMU.load_uint64(baseAddr + (i * nf + fn) * 8) : 0;
      is_zero = is_valid && p->VU.elt<uint64_t>(rd_num + fn, vreg_inx) == 0;
      break;
    }

    if (is_zero) {
      p->VU.vl = i;
      early_stop = true;
      break;
    }
  }

  if (early_stop) {
    break;
  }
}

p->VU.vstart = 0;
VI_CHECK_1905
