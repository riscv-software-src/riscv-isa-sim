// vrgather.vx vd, vs2, rs1, vm # vd[i] = (vs1[i] >= VLMAX) ? 0 : vs2[rs1];
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
reg_t vl = p->VU.vl;
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
reg_t rs1 = RS1;
for (reg_t i = P.VU.vstart ; i < vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, i) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint8_t>(rs2_num, rs1);
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, i) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint16_t>(rs2_num, rs1);
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, i) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint32_t>(rs2_num, rs1);
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i) = rs1 >= P.VU.vlmax ? 0 : P.VU.elt<uint64_t>(rs2_num, rs1);
    break;
  }
}

for (reg_t i = vl; i < P.VU.vlmax; ++i) {
   switch (sew) {
   case e8:
     P.VU.elt<uint8_t>(rd_num, i) = 0;
     break;
   case e16:
     P.VU.elt<uint16_t>(rd_num, i) = 0;
     break;
   case e32:
     P.VU.elt<uint32_t>(rd_num, i) = 0;
     break;
   default:
     P.VU.elt<uint64_t>(rd_num, i) = 0;
     break;
   }
}
VI_CHECK_1905
