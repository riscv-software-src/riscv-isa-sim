if (get_field(STATE.mstatus->read(), MSTATUS_TW)) {
  require_privilege(PRV_M);
} else if (STATE.v) {
  if (STATE.prv == PRV_U || get_field(STATE.hstatus->read(), HSTATUS_VTW))
    require_novirt();
} else if (p->extension_enabled('S')) {
  // When S-mode is implemented, then executing WFI in
  // U-mode causes an illegal instruction exception.
  require_privilege(PRV_S);
}
wfi();
