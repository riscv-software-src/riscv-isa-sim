require_privilege(get_field(STATE.mstatus, MSTATUS_TW) ? PRV_M : PRV_S);
wfi();
