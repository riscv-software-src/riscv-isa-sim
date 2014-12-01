int csr = validate_csr(insn.csr(), true);
reg_t old = p->get_pcr(csr);
p->set_pcr(csr, old & ~(reg_t)insn.rs1());
WRITE_RD(sext_xprlen(old));
