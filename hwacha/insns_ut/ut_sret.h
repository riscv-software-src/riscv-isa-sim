require_supervisor;
p->set_pcr(CSR_STATUS, ((p->get_state()->sr & ~(SR_S | SR_EI)) |
                       ((p->get_state()->sr & SR_PS) ? SR_S : 0)) |
                       ((p->get_state()->sr & SR_PEI) ? SR_EI : 0));
set_pc(p->get_state()->epc);
