// vmv_s_x: vd[0] = rs1
require(insn.v_vm() == 1);
require(p->VU.vsew == e8 || p->VU.vsew == e16 ||
        p->VU.vsew == e32 || p->VU.vsew == e64);
reg_t vl = p->VU.vl;

if (vl > 0) {
  reg_t rd_num = insn.rd();
  reg_t sew = p->VU.vsew;

  switch(sew) {
  case e8:
    p->VU.elt<uint8_t>(rd_num, 0) = RS1;
    break;
  case e16:
    p->VU.elt<uint16_t>(rd_num, 0) = RS1;
    break;
  case e32:
    p->VU.elt<uint32_t>(rd_num, 0) = RS1;
    break;
  default:
    p->VU.elt<uint64_t>(rd_num, 0) = RS1;
    break;
  }

  const reg_t max_len = p->VU.VLEN / sew;
  for (reg_t i = 1; i < max_len; ++i){
    switch(sew) {
    case e8:
      p->VU.elt<uint8_t>(rd_num, i) = 0;
      break;
    case e16:
      p->VU.elt<uint16_t>(rd_num, i) = 0;
      break;
    case e32:
      p->VU.elt<uint32_t>(rd_num, i) = 0;
      break;
    default:
      p->VU.elt<uint64_t>(rd_num, i) = 0;
      break;
    }
  }

  vl = 0;
}
