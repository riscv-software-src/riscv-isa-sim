bool write = insn.rs1() != 0;
int csr = validate_csr(insn.csr(), write);
reg_t old = p->get_csr(csr, insn, write);
if (write) {
  p->put_csr(csr, p->get_csr_for_write(csr) | RS1);
}
WRITE_RD(sext_xlen(old));
serialize();
