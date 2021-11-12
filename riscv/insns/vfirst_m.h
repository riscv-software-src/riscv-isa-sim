// vmfirst rd, vs2
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require_vector(true);
reg_t vl = p->VU.vl->read();
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();
require(p->VU.vstart->read() == 0);
reg_t pos = -1;
for (reg_t i=p->VU.vstart->read(); i < vl; ++i) {
  VI_LOOP_ELEMENT_SKIP()

  bool vs2_lsb = ((p->VU.elt<uint64_t>(rs2_num, midx ) >> mpos) & 0x1) == 1;
  if (vs2_lsb) {
    pos = i;
    break;
  }
}
p->VU.vstart->write(0);
WRITE_RD(pos);
