// vcompress vd, vs2, vs1
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector;
require(P.VU.vstart == 0);
reg_t sew = P.VU.vsew;
reg_t vl = P.VU.vl;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
reg_t pos = 0;
for (reg_t i = P.VU.vstart ; i < vl; ++i) {
  const int mlen = P.VU.vmlen;
  const int midx = (mlen * i) / 64;
  const int mpos = (mlen * i) % 64;

  bool do_mask = (P.VU.elt<uint64_t>(rs1_num, midx) >> mpos) & 0x1;
  if (do_mask) {
    switch (sew) {
    case e8:
      P.VU.elt<uint8_t>(rd_num, pos) = P.VU.elt<uint8_t>(rs2_num, i);
      break;
    case e16:
      P.VU.elt<uint16_t>(rd_num, pos) = P.VU.elt<uint16_t>(rs2_num, i);
      break;
    case e32:
      P.VU.elt<uint32_t>(rd_num, pos) = P.VU.elt<uint32_t>(rs2_num, i);
      break;
    default:
      P.VU.elt<uint64_t>(rd_num, pos) = P.VU.elt<uint64_t>(rs2_num, i);
      break;
    }

    ++pos;
  }
}

if (vl > 0 && TAIL_ZEROING) {
  uint8_t *tail = &P.VU.elt<uint8_t>(rd_num, pos * ((sew >> 3) * 1));
  memset(tail, 0, (P.VU.vlmax - pos) * ((sew >> 3) * 1));
}

