// vrgather.vi vd, vs2, zimm5 vm # vd[i] = (zimm5 >= VLMAX) ? 0 : vs2[zimm5];
require((insn.rd() & (P.VU.vlmul - 1)) == 0);
require((insn.rs2() & (P.VU.vlmul - 1)) == 0);
require(insn.rd() != insn.rs2());
if (insn.v_vm() == 0)
  require(insn.rd() != 0);

reg_t zimm5 = insn.v_zimm5();

VI_LOOP_BASE

for (reg_t i = P.VU.vstart; i < vl; ++i) {
  VI_LOOP_ELEMENT_SKIP();

  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, i) = zimm5 >= P.VU.vlmax ? 0 : P.VU.elt<uint8_t>(rs2_num, zimm5);
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, i) = zimm5 >= P.VU.vlmax ? 0 : P.VU.elt<uint16_t>(rs2_num, zimm5);
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, i) = zimm5 >= P.VU.vlmax ? 0 : P.VU.elt<uint32_t>(rs2_num, zimm5);
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i) = zimm5 >= P.VU.vlmax ? 0 : P.VU.elt<uint64_t>(rs2_num, zimm5);
    break;
  }
}

VI_LOOP_END;
