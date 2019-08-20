// vmpopc rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector;
reg_t vl = P.VU.vl;
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();

for (reg_t i = P.VU.vstart ; i < P.VU.vl; ++i) {
  VI_LOOP_ELEMENT_SKIP();

  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, i) = i;
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, i) = i;
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, i) = i;
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i) = i;
    break;
  }
}

VI_TAIL_ZERO(1);
P.VU.vstart = 0;
