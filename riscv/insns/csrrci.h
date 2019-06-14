bool write = insn.rs1() != 0;
int csr = validate_csr(insn.csr(), write);
reg_t old = p->get_csr(csr);
if (write) {
  p->set_csr(csr, old & ~(reg_t)insn.rs1());
}
WRITE_RD(sext_xlen(old));
serialize();
