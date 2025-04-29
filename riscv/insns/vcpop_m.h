// vmpopc rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t vl = P.VU.vl->read();
reg_t rs2_num = insn.rs2();
require(P.VU.vstart->read() == 0);
reg_t popcount = 0;
for (reg_t i=P.VU.vstart->read(); i<vl; ++i) {
  bool vs2_bit = P.VU.mask_elt(rs2_num, i);
  popcount += vs2_bit && (insn.v_vm() || P.VU.mask_elt(0, i));
}
WRITE_RD(popcount);
