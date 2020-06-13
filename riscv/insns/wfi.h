require_privilege(get_field(STATE.mstatus, MSTATUS_TW) ? PRV_M : PRV_S);
if (STATE.v && get_field(STATE.hstatus, HSTATUS_VTW))
  require_novirt();
wfi();
