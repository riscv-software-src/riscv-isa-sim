// vmpopc rd, vs2, vm
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require(!p->VU.vill);
reg_t vl = p->VU.vl;
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();

for (reg_t i = P.VU.vstart ; i < P.VU.vl; ++i) {
  V_LOOP_ELEMENT_SKIP;

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
VI_CHECK_1905
