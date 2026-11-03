// vcompress vd, vs2, vs1
require(P.any_vector_extensions());
require(P.VU.vstart->read() == 0);
require_align(insn.rd(), P.VU.vflmul);
require_align(insn.rs2(), P.VU.vflmul);
require(insn.rd() != insn.rs2());
require_noover(insn.rd(), P.VU.vflmul, insn.rs1(), 1);
// vs2 (EEW=SEW) is a src reg and shouln't overlap the mask source vs1 (EEW=1)
VI_CHECK_SOURCE_OVERLAP(insn.rs1(), 1, \
                        insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                        0, 1, 1, 0);

reg_t pos = 0;

VI_GENERAL_LOOP_BASE
  if (P.VU.mask_elt(rs1_num, i)) {
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
