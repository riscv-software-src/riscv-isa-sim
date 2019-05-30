// vle.v and vlseg[2-8]e.v
const reg_t sew = p->VU.vsew;
const reg_t nf = insn.v_nf() + 1;
const reg_t vl = p->VU.vl;
const reg_t elt_byte = sew / 8;
require(sew >= e8 && sew <= e64);
require((nf >= 2 && p->VU.vlmul == 1) || nf == 1);
reg_t baseAddr = RS1;
reg_t vd = insn.rd();
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  STRIP(i)
  V_ELEMENT_SKIP(i);
  for (reg_t fn = 0; fn < nf; ++fn) {
    switch (sew) {
    case e8:
      p->VU.elt<uint8_t>(vd + fn, vreg_inx) = is_valid ? 
          MMU.load_int8(baseAddr + (i * nf + fn) * elt_byte) : 0;
      break;
    case e16:
      p->VU.elt<uint16_t>(vd + fn, vreg_inx) = is_valid ?
          MMU.load_int16(baseAddr + (i * nf + fn) * elt_byte) : 0;
      break;
    case e32:
      p->VU.elt<uint32_t>(vd + fn, vreg_inx) = is_valid ?
          MMU.load_int32(baseAddr + (i * nf + fn) * elt_byte) : 0;
      break;
    case e64:
      p->VU.elt<uint64_t>(vd + fn, vreg_inx) =  is_valid ?
          MMU.load_int64(baseAddr + (i * nf + fn) * elt_byte) : 0;
      break;
    }
  }
}


p->VU.vstart = 0;
VI_CHECK_1905
