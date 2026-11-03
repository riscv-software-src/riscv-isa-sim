// vrgather.vx vd, vs2, rs1, vm # vd[i] = (rs1 >= VLMAX) ? 0 : vs2[rs1];
require_align(insn.rd(), P.VU.vflmul);
require_align(insn.rs2(), P.VU.vflmul);
require(insn.rd() != insn.rs2());
require_vm;
// vs2 is a data source (EEW=SEW); it must not overlap the v0 mask (EEW=1)
// when masked
VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                        insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                        0, 1, 1, 0);

reg_t rs1 = RS1;

VI_LOOP_BASE
  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint8_t>(rs2_num, rs1);
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint16_t>(rs2_num, rs1);
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint32_t>(rs2_num, rs1);
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint64_t>(rs2_num, rs1);
    break;
  }
VI_LOOP_END;
