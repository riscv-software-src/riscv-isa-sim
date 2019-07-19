// vmv_s_x: vd[0] = rs1
require_vector;
require(insn.v_vm() == 1);
require(P.VU.vsew == e8 || P.VU.vsew == e16 ||
        P.VU.vsew == e32 || P.VU.vsew == e64);
reg_t vl = P.VU.vl;

if (vl > 0) {
  reg_t rd_num = insn.rd();
  reg_t sew = P.VU.vsew;

  switch(sew) {
  case e8:
    P.VU.elt<uint8_t>(rd_num, 0) = RS1;
    break;
  case e16:
    P.VU.elt<uint16_t>(rd_num, 0) = RS1;
    break;
  case e32:
    P.VU.elt<uint32_t>(rd_num, 0) = RS1;
    break;
  default:
    P.VU.elt<uint64_t>(rd_num, 0) = RS1;
    break;
  }

  const reg_t max_len = P.VU.VLEN / sew;
  for (reg_t i = 1; i < max_len; ++i) {
    switch(sew) {
    case e8:
      P.VU.elt<uint8_t>(rd_num, i) = 0;
      break;
    case e16:
      P.VU.elt<uint16_t>(rd_num, i) = 0;
      break;
    case e32:
      P.VU.elt<uint32_t>(rd_num, i) = 0;
      break;
    default:
      P.VU.elt<uint64_t>(rd_num, i) = 0;
      break;
    }
  }

  vl = 0;
}
