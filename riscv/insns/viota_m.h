// vmpopc rd, vs2, vm
require(P.VU.vsew >= e8 && P.VU.vsew <= e64);
require_vector(true);
reg_t vl = P.VU.vl->read();
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs2_num = insn.rs2();
require(P.VU.vstart->read() == 0);
require_vm;
require_align(rd_num, P.VU.vflmul);
require_noover(rd_num, P.VU.vflmul, rs2_num, 1);

int cnt = 0;
for (reg_t i = 0; i < vl; ++i) {
  bool do_mask = P.VU.mask_elt(0, i);

  bool has_one = false;
  if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
    if (P.VU.mask_elt(rs2_num, i)) {
      has_one = true;
    }
  }

  // Bypass masked-off elements
  if ((insn.v_vm() == 0) && !do_mask)
    continue;

  switch (sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, i, true) = cnt;
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, i, true) = cnt;
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, i, true) = cnt;
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, i, true) = cnt;
    break;
  }

  if (has_one) {
    cnt++;
  }
}

