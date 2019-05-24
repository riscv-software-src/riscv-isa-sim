// vfmerge_vf vd, vs2, vs1, vm
require_extension('F');
require_fp;
require(P.VU.vsew == 32);
require(!P.VU.vill);
reg_t vl = P.VU.vl;
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
softfloat_roundingMode = STATE.frm;
for (reg_t i=P.VU.vstart; i<vl; ++i) {
  auto &vd = P.VU.elt<float32_t>(rd_num, i);
  auto rs1 = f32(READ_FREG(rs1_num));
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);
  bool do_mask = false;

  if (insn.v_vm() == 0) {
    int midx = (P.VU.vmlen * i) / 64;
    int mpos = (P.VU.vmlen * i) % 64;
    do_mask = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;
  }

  bool use_first = (insn.v_vm() == 1) || do_mask;
  vd = use_first ? rs1 : vs2;
}

VI_TAIL_ZERO(1);
P.VU.vstart = 0;
set_fp_exceptions;
