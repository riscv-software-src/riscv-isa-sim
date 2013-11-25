int csr = validate_csr(insn.i_imm(), true);
WRITE_RD(p->set_pcr(csr, p->get_pcr(csr) | insn.rs1()));
