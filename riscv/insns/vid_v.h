// vmpopc rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t vl = P.VU.vl;
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
require_align(rd_num, P.VU.vflmul);
require_vm;

for (reg_t i = P.VU.vstart ; i < P.VU.vl; ++i) {
  VI_LOOP_ELEMENT_SKIP();

  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, i, true) = i;
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, i, true) = i;
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, i, true) = i;
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i, true) = i;
    break;
  }
}

P.VU.vstart = 0;
