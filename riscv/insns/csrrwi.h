int csr = validate_csr(insn.csr(), true);
reg_t old;
if (check_vcsr(csr)) {
  old = STATE.VU.get_vcsr(csr);
  STATE.VU.set_vcsr(csr, insn.rs1());
}else{
  old = p->get_csr(csr);
  p->set_csr(csr, insn.rs1());
}
WRITE_RD(sext_xlen(old));
serialize();
