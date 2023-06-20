require_extension(EXT_SMRNMI);
require_privilege(PRV_M);
set_pc_and_serialize(p->get_state()->mnepc->read());
reg_t s = STATE.mnstatus->read();
reg_t prev_prv = get_field(s, MNSTATUS_MNPP);
reg_t prev_virt = get_field(s, MNSTATUS_MNPV);
if (prev_prv != PRV_M) {
  reg_t mstatus = STATE.mstatus->read();
  mstatus = set_field(mstatus, MSTATUS_MPRV, 0);
  STATE.mstatus->write(mstatus);
}
s = set_field(s, MNSTATUS_NMIE, 1);
STATE.mnstatus->write(s);
p->set_privilege(prev_prv, prev_virt);
