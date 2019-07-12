require(STATE.debug_mode);
set_pc_and_serialize(STATE.dpc);
p->set_privilege(STATE.dcsr.prv);

/* We're not in Debug Mode anymore. */
STATE.debug_mode = false;

if (STATE.dcsr.step)
  STATE.single_step = STATE.STEP_STEPPING;
