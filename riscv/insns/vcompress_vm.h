// vcompress vd, vs2, vs1
require(p->VU.vstart->read() == 0);
require_align(insn.rd(), p->VU.vflmul);
require_align(insn.rs2(), p->VU.vflmul);
require(insn.rd() != insn.rs2());
require_noover(insn.rd(), p->VU.vflmul, insn.rs1(), 1);

reg_t pos = 0;

VI_GENERAL_LOOP_BASE
  const int midx = i / 64;
  const int mpos = i % 64;

  bool do_mask = (p->VU.elt<uint64_t>(rs1_num, midx) >> mpos) & 0x1;
  if (do_mask) {
    switch (sew) {
    case e8:
      p->VU.elt<uint8_t>(rd_num, pos, true) = p->VU.elt<uint8_t>(rs2_num, i);
      break;
    case e16:
      p->VU.elt<uint16_t>(rd_num, pos, true) = p->VU.elt<uint16_t>(rs2_num, i);
      break;
    case e32:
      p->VU.elt<uint32_t>(rd_num, pos, true) = p->VU.elt<uint32_t>(rs2_num, i);
      break;
    default:
      p->VU.elt<uint64_t>(rd_num, pos, true) = p->VU.elt<uint64_t>(rs2_num, i);
      break;
    }

    ++pos;
  }
VI_LOOP_END;
