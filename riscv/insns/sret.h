require_extension('S');
if (STATE.v) {
  if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTSR))
    require_novirt();
} else {
  require_privilege(get_field(STATE.mstatus->read(), MSTATUS_TSR) ? PRV_M : PRV_S);
}
reg_t next_pc = p->get_state()->sepc->read();
set_pc_and_serialize(next_pc);
reg_t s = STATE.sstatus->read();
reg_t prev_prv = get_field(s, MSTATUS_SPP);
if (prev_prv != PRV_M)
  s = set_field(s, MSTATUS_MPRV, 0);
s = set_field(s, MSTATUS_SIE, get_field(s, MSTATUS_SPIE));
s = set_field(s, MSTATUS_SPIE, 1);
s = set_field(s, MSTATUS_SPP, PRV_U);
STATE.sstatus->write(s);
p->set_privilege(prev_prv);
if (!STATE.v) {
  reg_t prev_virt = get_field(STATE.hstatus->read(), HSTATUS_SPV);
  p->set_virt(prev_virt);
}
