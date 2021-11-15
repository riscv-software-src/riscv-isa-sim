// vrgatherei16.vv vd, vs2, vs1, vm # vd[i] = (vs1[i] >= VLMAX) ? 0 : vs2[vs1[i]];
float vemul = (16.0 / p->VU.vsew * p->VU.vflmul);
require(vemul >= 0.125 && vemul <= 8);
require_align(insn.rd(), p->VU.vflmul);
require_align(insn.rs2(), p->VU.vflmul);
require_align(insn.rs1(), vemul);
require_noover(insn.rd(), p->VU.vflmul, insn.rs1(), vemul);
require(insn.rd() != insn.rs2());
require_vm;

VI_LOOP_BASE
  switch (sew) {
  case e8: {
    auto vs1 = p->VU.elt<uint16_t>(rs1_num, i);
    p->VU.elt<uint8_t>(rd_num, i, true) = vs1 >= p->VU.vlmax ? 0 : p->VU.elt<uint8_t>(rs2_num, vs1);
    break;
  }
  case e16: {
    auto vs1 = p->VU.elt<uint16_t>(rs1_num, i);
    p->VU.elt<uint16_t>(rd_num, i, true) = vs1 >= p->VU.vlmax ? 0 : p->VU.elt<uint16_t>(rs2_num, vs1);
    break;
  }
  case e32: {
    auto vs1 = p->VU.elt<uint16_t>(rs1_num, i);
    p->VU.elt<uint32_t>(rd_num, i, true) = vs1 >= p->VU.vlmax ? 0 : p->VU.elt<uint32_t>(rs2_num, vs1);
    break;
  }
  default: {
    auto vs1 = p->VU.elt<uint16_t>(rs1_num, i);
    p->VU.elt<uint64_t>(rd_num, i, true) = vs1 >= p->VU.vlmax ? 0 : p->VU.elt<uint64_t>(rs2_num, vs1);
    break;
  }
  }
VI_LOOP_END;
