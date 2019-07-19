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

  vd = rs1;
}

VI_TAIL_ZERO(1);
P.VU.vstart = 0;
set_fp_exceptions;
