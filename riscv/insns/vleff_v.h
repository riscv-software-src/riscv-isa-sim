// vle.v and vlseg[2-8]e.v
const reg_t sew = p->VU.vsew;
const reg_t elt_byte = sew / 8;
require(sew >= e8 && sew <= e64);
reg_t vl = p->VU.vl;
reg_t baseAddr = RS1;
reg_t rd_num = insn.rd();
bool early_stop = false;
reg_t vlmax = p->VU.vlmax;
for (reg_t i = 0; i < vlmax && vl != 0; ++i) {
  bool is_valid = true;
  STRIP(i)
  V_ELEMENT_SKIP(i);

  switch (sew) {
  case e8: {
      auto val = MMU.load_int8(baseAddr + i * elt_byte);
      p->VU.elt<uint8_t>(rd_num, i) = is_valid ? val : 0;

      if (val == 0) {
        early_stop = true;
      }
    }
    break;
  case e16: {
      auto val = MMU.load_int16(baseAddr + i * elt_byte);
      p->VU.elt<uint16_t>(rd_num, i) = is_valid ? val : 0;

      if (val == 0) {
        early_stop = true;
      }
    }
    break;
  case e32: {
      auto val = MMU.load_int32(baseAddr + i * elt_byte);
      p->VU.elt<uint32_t>(rd_num, i) = is_valid ? val : 0;

      if (val == 0) {
        early_stop = true;
      }
    }
    break;
  default: {
      auto val = MMU.load_int64(baseAddr + i * elt_byte);
      p->VU.elt<uint64_t>(rd_num, i) = is_valid ? val : 0;

      if (val == 0) {
        early_stop = true;
      }
    }
    break;
  }

  if (early_stop) {
    p->VU.vl = i;
    break;
  }
}

if (!early_stop) {
  VI_TAIL_ZERO(1);
}
p->VU.vstart = 0;
VI_CHECK_1905
