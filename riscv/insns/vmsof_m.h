// vmsof.m rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
require(P.VU.vstart->read() == 0);
require_vm;
require(insn.rd() != insn.rs2());

reg_t vl = P.VU.vl->read();
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();

bool has_one = false;
for (reg_t i = P.VU.vstart->read() ; i < vl; ++i) {
  bool vs2_lsb = P.VU.mask_elt(rs2_num, i);
  bool do_mask = P.VU.mask_elt(0, i);

  if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
    bool res = false;
    if (!has_one && vs2_lsb) {
      has_one = true;
      res = true;
    }

    P.VU.set_mask_elt(rd_num, i, res);
  }
}
