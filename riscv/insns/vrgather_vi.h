// vrgather.vi vd, vs2, zimm5 vm # vd[i] = (zimm5 >= VLMAX) ? 0 : vs2[zimm5];
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
reg_t vl = p->VU.vl;
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();
reg_t zimm5 = insn.v_zimm5();
for (reg_t i = P.VU.vstart; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

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

VI_TAIL_ZERO(1);
P.VU.vstart = 0;
VI_CHECK_1905
