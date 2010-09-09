require_supervisor;

switch(insn.rtype.rb)
{
  case 0:
    set_sr(RA);
    break;
  case 1:
    epc = RA;
    break;
  case 3:
    ebase = RA & ~0xFFF;
    break;
  case 4:
    count = RA;
    break;
  case 5:
    interrupts_pending &= ~(1 << TIMER_IRQ);
    compare = RA;
    break;

  case 16:
    tohost = RA;
    sim->set_tohost(RA);
    break;

  case 24:
    pcr_k0 = RA;
    break;
  case 25:
    pcr_k1 = RA;
    break;
}
