// vmv_s_x: vd[0] = rs1
require_vector(true);
require(insn.v_vm() == 1);
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
reg_t vl = p->VU.vl->read();

if (vl > 0 && p->VU.vstart->read() < vl) {
  reg_t rd_num = insn.rd();
  reg_t sew = p->VU.vsew;

  switch(sew) {
  case e8:
    p->VU.elt<uint8_t>(rd_num, 0, true) = RS1;
    break;
  case e16:
    p->VU.elt<uint16_t>(rd_num, 0, true) = RS1;
    break;
  case e32:
    p->VU.elt<uint32_t>(rd_num, 0, true) = RS1;
    break;
  default:
    p->VU.elt<uint64_t>(rd_num, 0, true) = RS1;
    break;
  }

  vl = 0;
}

p->VU.vstart->write(0);
