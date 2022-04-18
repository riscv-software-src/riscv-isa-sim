// vmv_x_s: rd = vs2[0]
require_vector(true);
require(insn.v_vm() == 1);
uint64_t xmask = UINT64_MAX >> (64 - P.get_isa().get_max_xlen());
reg_t rs1 = RS1;
reg_t sew = P.VU.vsew;
reg_t rs2_num = insn.rs2();

switch(sew) {
case e8:
  WRITE_RD(P.VU.elt<int8_t>(rs2_num, 0));
  break;
case e16:
  WRITE_RD(P.VU.elt<int16_t>(rs2_num, 0));
  break;
case e32:
  WRITE_RD(P.VU.elt<int32_t>(rs2_num, 0));
  break;
case e64:
  if (P.get_isa().get_max_xlen() <= sew)
    WRITE_RD(P.VU.elt<uint64_t>(rs2_num, 0) & xmask);
  else
    WRITE_RD(P.VU.elt<uint64_t>(rs2_num, 0));
  break;
}

P.VU.vstart->write(0);
