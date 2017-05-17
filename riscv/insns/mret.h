require_privilege(PRV_M);
set_pc_and_serialize(p->get_state()->mepc);
reg_t s = STATE.mstatus;
reg_t prev_prv = get_field(s, MSTATUS_MPP);
s = set_field(s, MSTATUS_MIE, get_field(s, MSTATUS_MPIE));
s = set_field(s, MSTATUS_MPIE, 1);
s = set_field(s, MSTATUS_MPP, PRV_U);
p->set_privilege(prev_prv);
p->set_csr(CSR_MSTATUS, s);
