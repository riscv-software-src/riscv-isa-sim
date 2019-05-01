int csr = validate_csr(insn.csr(), true);
reg_t old;
if (check_vcsr(csr)) {
  old = p->VU.get_vcsr(csr);
  p->VU.set_vcsr(csr, insn.rs1());
}else{
  old = p->get_csr(csr);
  p->set_csr(csr, insn.rs1());
}
WRITE_RD(sext_xlen(old));
serialize();
