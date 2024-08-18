require_privilege(PRV_M);
set_pc_and_serialize(p->get_state()->mepc->read());
reg_t s = STATE.mstatus->read();
reg_t prev_prv = get_field(s, MSTATUS_MPP);
reg_t prev_virt = get_field(s, MSTATUS_MPV);
if (prev_prv != PRV_M)
  s = set_field(s, MSTATUS_MPRV, 0);
s = set_field(s, MSTATUS_MIE, get_field(s, MSTATUS_MPIE));
s = set_field(s, MSTATUS_MPIE, 1);
s = set_field(s, MSTATUS_MPP, p->extension_enabled('U') ? PRV_U : PRV_M);
s = set_field(s, MSTATUS_MPV, 0);
if (ZICFILP_xLPE(prev_virt, prev_prv)) {
  STATE.elp = static_cast<elp_t>(get_field(s, MSTATUS_MPELP));
}
s = set_field(s, MSTATUS_MPELP, elp_t::NO_LP_EXPECTED);
s = set_field(s, MSTATUS_MDT, 0);
if (prev_prv == PRV_U || prev_virt)
  s = set_field(s, MSTATUS_SDT, 0);
if (prev_virt && prev_prv == PRV_U)
  STATE.vsstatus->write(STATE.vsstatus->read() & ~SSTATUS_SDT);
STATE.mstatus->write(s);
if (STATE.mstatush) STATE.mstatush->write(s >> 32); // log mstatush change
STATE.tcontrol->write((STATE.tcontrol->read() & CSR_TCONTROL_MPTE) ? (CSR_TCONTROL_MPTE | CSR_TCONTROL_MTE) : 0);
p->set_privilege(prev_prv, prev_virt);
