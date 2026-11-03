// vrgather.vv vd, vs2, vs1, vm # vd[i] = (vs1[i] >= VLMAX) ? 0 : vs2[vs1[i]];
require_align(insn.rd(), P.VU.vflmul);
require_align(insn.rs2(), P.VU.vflmul);
require_align(insn.rs1(), P.VU.vflmul);
require(insn.rd() != insn.rs2() && insn.rd() != insn.rs1());
require_vm;
// vs2 and vs1 are data sources (EEW=SEW); they must not overlap the v0 mask
// (EEW=1) when masked, nor each other unless their EEWs are equal
VI_CHECK_SOURCE_OVERLAP(MASK_IS_V0, 1, \
                        insn.rs2(), P.VU.vflmul, P.VU.vsew, \
                        insn.rs1(), P.VU.vflmul, P.VU.vsew, 1);

VI_LOOP_BASE
  switch (sew) {
  case e8: {
    auto vs1 = P.VU.elt<uint8_t>(rs1_num, i);
    //if (i > 255) continue;
    P.VU.elt<uint8_t>(rd_num, i, true) = vs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint8_t>(rs2_num, vs1);
    break;
  }
  case e16: {
    auto vs1 = P.VU.elt<uint16_t>(rs1_num, i);
    P.VU.elt<uint16_t>(rd_num, i, true) = vs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint16_t>(rs2_num, vs1);
    break;
  }
  case e32: {
    auto vs1 = P.VU.elt<uint32_t>(rs1_num, i);
    P.VU.elt<uint32_t>(rd_num, i, true) = vs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint32_t>(rs2_num, vs1);
    break;
  }
  default: {
    auto vs1 = P.VU.elt<uint64_t>(rs1_num, i);
    P.VU.elt<uint64_t>(rd_num, i, true) = vs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint64_t>(rs2_num, vs1);
    break;
  }
  }
VI_LOOP_END;
