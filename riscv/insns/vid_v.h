// vmpopc rd, vs2, vm
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require_vector(true);
reg_t vl = p->VU.vl->read();
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
require_align(rd_num, p->VU.vflmul);
require_vm;

for (reg_t i = p->VU.vstart->read() ; i < p->VU.vl->read(); ++i) {
  VI_LOOP_ELEMENT_SKIP();

  switch (sew) {
  case e8:
    p->VU.elt<uint8_t>(rd_num, i, true) = i;
    break;
  case e16:
    p->VU.elt<uint16_t>(rd_num, i, true) = i;
    break;
  case e32:
    p->VU.elt<uint32_t>(rd_num, i, true) = i;
    break;
  default:
    p->VU.elt<uint64_t>(rd_num, i, true) = i;
    break;
  }
}

p->VU.vstart->write(0);
