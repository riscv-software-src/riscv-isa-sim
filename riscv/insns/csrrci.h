int csr = validate_csr(insn.csr(), true);
WRITE_RD(p->set_pcr(csr, p->get_pcr(csr) & ~(reg_t)insn.rs1()));
