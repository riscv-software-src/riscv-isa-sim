// vcompress vd, vs2, vs1
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
require(p->VU.vstart == 0);
reg_t sew = p->VU.vsew;
reg_t vl = p->VU.vl;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
reg_t pos = 0;
for (reg_t i = 0 ; i < vl; ++i) {
  const int mlen = p->VU.vmlen;
  const int midx = (mlen * i) / 64;
  const int mpos = (mlen * i) % 64;

  bool do_mask = (p->VU.elt<uint64_t>(rs1_num, midx) >> mpos) & 0x1;
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

if (vl > 0) {
  uint8_t *tail = &P.VU.elt<uint8_t>(rd_num, pos * ((sew >> 3) * 1));
  memset(tail, 0, (P.VU.vlmax - pos) * ((sew >> 3) * 1));
}

VI_CHECK_1905
