if (STATE.v && STATE.prv == PRV_U) {
  require_novirt();
} else if (get_field(STATE.mstatus->read(), MSTATUS_TW)) {
  require_privilege(PRV_M);
} else if (STATE.v) { // VS-mode
  if (get_field(STATE.hstatus->read(), HSTATUS_VTW))
    require_novirt();
} else {
  require_privilege(PRV_S);
}
wfi();
