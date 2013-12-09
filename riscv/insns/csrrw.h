int csr = validate_csr(insn.csr(), true);
WRITE_RD(p->set_pcr(csr, RS1));
