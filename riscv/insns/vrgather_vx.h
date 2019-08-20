// vrgather.vx vd, vs2, rs1, vm # vd[i] = (rs1 >= VLMAX) ? 0 : vs2[rs1];
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector;
reg_t vl = P.VU.vl;
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
reg_t rs1 = RS1;
for (reg_t i = P.VU.vstart; i < vl; ++i) {
  VI_LOOP_ELEMENT_SKIP();

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

VI_TAIL_ZERO(1);
P.VU.vstart = 0;
