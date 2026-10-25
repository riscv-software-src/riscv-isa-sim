// RISC-V semihosting sequence
//      slli x0, x0, 0x1f    # 0x01f01013 Entry NOP
//      ebreak               # 0x00100073 Break to debugger
//      srai x0, x0, 7       # 0x40705013 Exit NOP
auto prev = MMU.load_insn(pc - 4);
auto next = MMU.load_insn(pc + 4);

if (prev.insn.bits() == 0x01f01013 && next.insn.bits() == 0x40705013) {
  // a0: the semihosting operation code
  // a1: the pointer to the semihosting operation arguments
  reg_t ret = p->semihosting(xlen, READ_REG(10), READ_REG(11));
  WRITE_REG(10, ret);   // Write the return value to register a0
} else if (!STATE.debug_mode && (
        (!STATE.v && STATE.prv == PRV_M && STATE.dcsr->ebreakm) ||
        (!STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreaks) ||
        (!STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreaku) ||
        (STATE.v && STATE.prv == PRV_S && STATE.dcsr->ebreakvs) ||
        (STATE.v && STATE.prv == PRV_U && STATE.dcsr->ebreakvu))) {
	throw trap_debug_mode();
} else {
	throw trap_breakpoint(STATE.v, pc);
}
