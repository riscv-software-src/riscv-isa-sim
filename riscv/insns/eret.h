require_supervisor;
set_pcr(PCR_SR, ((sr & ~(SR_S | SR_EI)) |
                ((sr & SR_PS) ? 0 : SR_S)) |
                ((sr & SR_PEI) ? SR_EI : 0));
set_pc(epc);
