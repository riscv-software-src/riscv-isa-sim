// vcompress vd, vs2, vs1
require(P.VU.vstart == 0);
require((insn.rd() & (P.VU.vlmul - 1)) == 0);
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);
require(insn.rd() != insn.rs2());
require(!is_overlapped(insn.rd(), P.VU.vlmul, insn.rs1(), 1));

reg_t pos = 0;

VI_GENERAL_LOOP_BASE
  const int mlen = P.VU.vmlen;
  const int midx = (mlen * i) / 64;
  const int mpos = (mlen * i) % 64;

  bool do_mask = (P.VU.elt<uint64_t>(rs1_num, midx) >> mpos) & 0x1;
  if (do_mask) {
    switch (sew) {
    case e8:
      P.VU.elt<uint8_t>(rd_num, pos, true) = P.VU.elt<uint8_t>(rs2_num, i);
      break;
    case e16:
      P.VU.elt<uint16_t>(rd_num, pos, true) = P.VU.elt<uint16_t>(rs2_num, i);
      break;
    case e32:
      P.VU.elt<uint32_t>(rd_num, pos, true) = P.VU.elt<uint32_t>(rs2_num, i);
      break;
    default:
      P.VU.elt<uint64_t>(rd_num, pos, true) = P.VU.elt<uint64_t>(rs2_num, i);
      break;
    }

    ++pos;
  }
VI_LOOP_END;
