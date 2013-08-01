require_supervisor;
set_pcr(PCR_SR, ((sr & ~(SR_S | SR_EI)) |
                ((sr & SR_PS) ? SR_S : 0)) |
                ((sr & SR_PEI) ? SR_EI : 0));
set_pc(epc);
