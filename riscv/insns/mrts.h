require_privilege(PRV_M);
p->set_csr(CSR_MSTATUS, set_field(STATE.mstatus, MSTATUS_PRV, PRV_S));
STATE.sbadaddr = STATE.mbadaddr;
STATE.scause = STATE.mcause;
STATE.sepc = STATE.mepc;
set_pc(STATE.stvec);
