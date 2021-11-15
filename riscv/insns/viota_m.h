// vmpopc rd, vs2, vm
require(p->VU.vsew >= e8 && p->VU.vsew <= e64);
require_vector(true);
reg_t vl = p->VU.vl->read();
reg_t sew = p->VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
require(p->VU.vstart->read() == 0);
require_vm;
require_align(rd_num, p->VU.vflmul);
require_noover(rd_num, p->VU.vflmul, rs2_num, 1);

int cnt = 0;
for (reg_t i = 0; i < vl; ++i) {
  const int midx = i / 64;
  const int mpos = i % 64;

  bool vs2_lsb = ((p->VU.elt<uint64_t>(rs2_num, midx) >> mpos) & 0x1) == 1;
  bool do_mask = (p->VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

  bool has_one = false;
  if (insn.v_vm() == 1 || (insn.v_vm() == 0 && do_mask)) {
    if (vs2_lsb) {
      has_one = true;
    }
  }

  bool use_ori = (insn.v_vm() == 0) && !do_mask;
  switch (sew) {
  case e8:
    p->VU.elt<uint8_t>(rd_num, i, true) = use_ori ?
                                   p->VU.elt<uint8_t>(rd_num, i) : cnt;
    break;
  case e16:
    p->VU.elt<uint16_t>(rd_num, i, true) = use_ori ?
                                    p->VU.elt<uint16_t>(rd_num, i) : cnt;
    break;
  case e32:
    p->VU.elt<uint32_t>(rd_num, i, true) = use_ori ?
                                    p->VU.elt<uint32_t>(rd_num, i) : cnt;
    break;
  default:
    p->VU.elt<uint64_t>(rd_num, i, true) = use_ori ?
                                    p->VU.elt<uint64_t>(rd_num, i) : cnt;
    break;
  }

  if (has_one) {
    cnt++;
  }
}

