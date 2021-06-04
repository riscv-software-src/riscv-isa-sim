require_extension('S');
if (STATE.v) {
  if (STATE.prv == PRV_U || get_field(STATE.hstatus, HSTATUS_VTSR))
    require_novirt();
} else {
  require_privilege(get_field(STATE.mstatus, MSTATUS_TSR) ? PRV_M : PRV_S);
}
reg_t next_pc = (STATE.v) ? p->get_state()->vsepc : p->get_state()->sepc;
set_pc_and_serialize(next_pc);
reg_t s = STATE.mstatus;
reg_t prev_prv = get_field(s, MSTATUS_SPP);
if (prev_prv != PRV_M)
  s = set_field(s, MSTATUS_MPRV, 0);
s = set_field(s, MSTATUS_SIE, get_field(s, MSTATUS_SPIE));
s = set_field(s, MSTATUS_SPIE, 1);
s = set_field(s, MSTATUS_SPP, PRV_U);
p->set_csr(CSR_MSTATUS, s);
p->set_privilege(prev_prv);
if (!STATE.v) {
  reg_t prev_virt = get_field(STATE.hstatus, HSTATUS_SPV);
  p->set_virt(prev_virt);
}
