int csr = validate_csr(insn.csr(), insn.rs1() != 0);
WRITE_RD(sext_xprlen(p->set_pcr(csr, p->get_pcr(csr) | RS1)));
