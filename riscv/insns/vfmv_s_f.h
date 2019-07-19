// vfmv_s_f: vd[0] = rs1 (vs2=0)
require_vector;
require(insn.v_vm() == 1);
require_fp;
require_extension('F');
require(P.VU.vsew == e32);
reg_t vl = P.VU.vl;

if (vl > 0) {
  reg_t rd_num = insn.rd();
  reg_t sew = P.VU.vsew;

  if (FLEN == 64)
    P.VU.elt<uint32_t>(rd_num, 0) = f64(FRS1).v;
  else
    P.VU.elt<uint32_t>(rd_num, 0) = f32(FRS1).v;

  const reg_t max_len = P.VU.VLEN / sew;
  for (reg_t i = 1; i < max_len; ++i) {
    switch(sew) {
    case e32:
      P.VU.elt<uint32_t>(rd_num, i) = 0;
      break;
    default:
      require(false);
      break;
    }
  }

  vl = 0;
}
