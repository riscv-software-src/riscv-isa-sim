bool write = insn.rs1() != 0;
int csr = validate_csr(insn.csr(), write);
reg_t old;
if (check_vcsr(csr)) {
  old = STATE.VU.get_vcsr(csr);
}else{
  old = p->get_csr(csr);
}

if (write) {
  p->set_csr(csr, old | RS1);
}
WRITE_RD(sext_xlen(old));
serialize();
