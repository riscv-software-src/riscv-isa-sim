// vfmerge_vf vd, vs2, vs1, vm
require_extension('F');
require_fp;
require(P.VU.vsew == 32);
require_vector;
reg_t vl = P.VU.vl;
reg_t sew = P.VU.vsew;
reg_t rd_num = insn.rd();
reg_t rs1_num = insn.rs1();
reg_t rs2_num = insn.rs2();
for (reg_t i=P.VU.vstart; i<vl; ++i) {
  auto &vd = P.VU.elt<float32_t>(rd_num, i);
  auto rs1 = f32(READ_FREG(rs1_num));
  auto vs2 = P.VU.elt<float32_t>(rs2_num, i);

  int midx = (P.VU.vmlen * i) / 64;
  int mpos = (P.VU.vmlen * i) % 64;
  bool use_first = (P.VU.elt<uint64_t>(0, midx) >> mpos) & 0x1;

  vd = use_first ? rs1 : vs2;
}

VI_TAIL_ZERO(1);
P.VU.vstart = 0;
set_fp_exceptions;
