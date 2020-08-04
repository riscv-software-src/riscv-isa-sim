// vmv_x_s: rd = vs2[rs1]
require_vector(true);
require(insn.v_vm() == 1);
uint64_t xmask = UINT64_MAX >> (64 - P.get_max_xlen());
reg_t rs1 = RS1;
reg_t sew = P.VU.vsew;
reg_t rs2_num = insn.rs2();

if (!(rs1 >= 0 && rs1 < (P.VU.get_vlen() / sew))) {
  WRITE_RD(0);
} else {
  switch(sew) {
  case e8:
    WRITE_RD(P.VU.elt<int8_t>(rs2_num, rs1));
    break;
  case e16:
    WRITE_RD(P.VU.elt<int16_t>(rs2_num, rs1));
    break;
  case e32:
    WRITE_RD(P.VU.elt<int32_t>(rs2_num, rs1));
    break;
  case e64:
    if (P.get_max_xlen() <= sew)
      WRITE_RD(P.VU.elt<uint64_t>(rs2_num, rs1) & xmask);
    else
      WRITE_RD(P.VU.elt<uint64_t>(rs2_num, rs1));
    break;
  }
}

P.VU.vstart = 0;
