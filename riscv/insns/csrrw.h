int csr = validate_csr(insn.csr(), true);
reg_t old;
if (check_vcsr(csr)) {
  old = P.VU.get_vcsr(csr);
  P.VU.set_vcsr(csr, RS1);
}else{
  old = p->get_csr(csr);
  p->set_csr(csr, RS1);
}
WRITE_RD(sext_xlen(old));
serialize();
