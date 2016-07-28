require_privilege(PRV_M);
set_pc_and_serialize(STATE.dpc);
/* The debug spec says we can't crash when prv is set to an invalid value. */
if (p->validate_priv(STATE.dcsr.prv)) {
  p->set_privilege(STATE.dcsr.prv);
}

/* We're not in Debug Mode anymore. */
STATE.dcsr.cause = 0;

if (STATE.dcsr.step)
  STATE.single_step = STATE.STEP_STEPPING;
