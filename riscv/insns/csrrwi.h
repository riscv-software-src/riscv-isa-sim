int csr = validate_csr(insn.csr(), true);
reg_t old = p->get_csr(csr, insn, true);
p->set_csr(csr, insn.rs1());
WRITE_RD(sext_xlen(old));
serialize();
