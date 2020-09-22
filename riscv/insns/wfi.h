if (STATE.v && (STATE.prv == PRV_U || get_field(STATE.hstatus, HSTATUS_VTW)))
  require_novirt();
require_privilege(get_field(STATE.mstatus, MSTATUS_TW) ? PRV_M : PRV_S);
wfi();
