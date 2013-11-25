int csr = validate_csr(insn.i_imm(), insn.rs1() != 0);
WRITE_RD(p->set_pcr(csr, p->get_pcr(csr) | RS1));
