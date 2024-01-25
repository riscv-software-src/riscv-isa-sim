// vmfirst rd, vs2
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t vl = P.VU.vl->read();
reg_t rs2_num = insn.rs2();
require(P.VU.vstart->read() == 0);
reg_t pos = -1;
for (reg_t i=P.VU.vstart->read(); i < vl; ++i) {
  VI_LOOP_ELEMENT_SKIP()

  bool vs2_lsb = ((P.VU.elt<uint64_t>(rs2_num, midx ) >> mpos) & 0x1) == 1;
  if (vs2_lsb) {
    pos = i;
    break;
  }
}
WRITE_RD(pos);
