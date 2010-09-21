require_supervisor;

switch(insn.rtype.rs2)
{
  case 0:
    set_sr(RS1);
    break;
  case 1:
    epc = RS1;
    break;
  case 3:
    ebase = RS1 & ~0xFFF;
    break;
  case 4:
    count = RS1;
    break;
  case 5:
    interrupts_pending &= ~(1 << TIMER_IRQ);
    compare = RS1;
    break;

  case 16:
    tohost = RS1;
    sim->set_tohost(RS1);
    break;

  case 24:
    pcr_k0 = RS1;
    break;
  case 25:
    pcr_k1 = RS1;
    break;
}
