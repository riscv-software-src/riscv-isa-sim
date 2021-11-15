// vrgather.vi vd, vs2, zimm5 vm # vd[i] = (zimm5 >= VLMAX) ? 0 : vs2[zimm5];
require_align(insn.rd(), p->VU.vflmul);
require_align(insn.rs2(), p->VU.vflmul);
require(insn.rd() != insn.rs2());
require_vm;

reg_t zimm5 = insn.v_zimm5();

VI_LOOP_BASE

for (reg_t i = p->VU.vstart->read(); i < vl; ++i) {
  VI_LOOP_ELEMENT_SKIP();

  switch (sew) {
  case e8:
    p->VU.elt<uint8_t>(rd_num, i, true) = zimm5 >= p->VU.vlmax ? 0 : p->VU.elt<uint8_t>(rs2_num, zimm5);
    break;
  case e16:
    p->VU.elt<uint16_t>(rd_num, i, true) = zimm5 >= p->VU.vlmax ? 0 : p->VU.elt<uint16_t>(rs2_num, zimm5);
    break;
  case e32:
    p->VU.elt<uint32_t>(rd_num, i, true) = zimm5 >= p->VU.vlmax ? 0 : p->VU.elt<uint32_t>(rs2_num, zimm5);
    break;
  default:
    p->VU.elt<uint64_t>(rd_num, i, true) = zimm5 >= p->VU.vlmax ? 0 : p->VU.elt<uint64_t>(rs2_num, zimm5);
    break;
  }
}

VI_LOOP_END;
