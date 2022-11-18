require_extension(EXT_ZCA);
if (!STATE.debug_mode &&
    ((STATE.prv == PRV_M && STATE.dcsr->ebreakm) ||
     (STATE.prv == PRV_S && STATE.dcsr->ebreaks) ||
     (STATE.prv == PRV_U && STATE.dcsr->ebreaku))) {
	throw trap_debug_mode();
} else {
	throw trap_breakpoint(STATE.v, pc);
}
