if (get_field(STATE.mstatus->read(), MSTATUS_TW)) {
  require_privilege(PRV_M);
} else if (STATE.v) {
  if (get_field(STATE.hstatus->read(), HSTATUS_VTW))
    require_novirt();
}
