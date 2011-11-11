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
    evec = RS1;
    break;
  case 4:
    count = RS1;
    break;
  case 5:
    interrupts_pending &= ~(1 << TIMER_IRQ);
    compare = RS1;
    break;
  case 7:
    mmu.set_ptbr(RS1);
    break;

  case 8:
    sim.send_ipi(RS1);
    break;
  case 9:
    interrupts_pending &= ~(1 << IPI_IRQ);
    break;

  case 12:
    pcr_k0 = RS1;
    break;
  case 13:
    pcr_k1 = RS1;
    break;

  case 16:
    sim.set_tohost(RS1);
    break;

  case 18:
    vecbanks = RS1 & 0xff;
    vecbanks_count = __builtin_popcountll(vecbanks);
    break;
}
