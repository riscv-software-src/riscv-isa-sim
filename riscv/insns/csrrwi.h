int csr = validate_csr(insn.csr(), true);
bool read = insn.rd() != 0;
reg_t old = 0;
if (read) {
  old = p->get_csr(csr, insn, true);
}
p->put_csr(csr, insn.rs1());
WRITE_RD(sext_xlen(old));
serialize();
