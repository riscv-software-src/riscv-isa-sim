// vrgather.vx vd, vs2, rs1, vm # vd[i] = (rs1 >= VLMAX) ? 0 : vs2[rs1];
require_align(insn.rd(), P.VU.vflmul);
require_align(insn.rs2(), P.VU.vflmul);
require(insn.rd() != insn.rs2());
require_vm;

reg_t rs1 = RS1;

VI_LOOP_BASE
  if (0 == P.VU.vta && i >= vl) { \
    continue; \
  } \
  if ((true == skip && 1 == P.VU.vma && i < vl) || (1 == P.VU.vta && i >= vl)) \
    mata_action = 2; \
  else \
    mata_action = 1; \
  switch (sew) {
  case e8:
    if (1 == mata_action) \
      P.VU.elt<uint8_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint8_t>(rs2_num, rs1);
    else \
      P.VU.elt<uint8_t>(rd_num, i, true) = 0xFF; \
    break;
  case e16:
    if (1 == mata_action) \
      P.VU.elt<uint16_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint16_t>(rs2_num, rs1);
    else \
      P.VU.elt<uint16_t>(rd_num, i, true) = 0xFFFF; \
    break;
  case e32:
    if (1 == mata_action) \
      P.VU.elt<uint32_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint32_t>(rs2_num, rs1);
    else \
      P.VU.elt<uint32_t>(rd_num, i, true) = 0xFFFFFFFF; \
    break;
  default:
    if (1 == mata_action) \
      P.VU.elt<uint64_t>(rd_num, i, true) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint64_t>(rs2_num, rs1);
    else \
      P.VU.elt<uint64_t>(rd_num, i, true) = 0xFFFFFFFFFFFFFFFF; \
    break;
  }
VI_LOOP_END;
