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
    cause &= ~(1 << (TIMER_IRQ+CAUSE_IP_SHIFT));
    compare = RS1;
    break;

  case 7:
    sim->send_ipi(RS1);
    break;

  case 9:
    mmu.set_ptbr(RS1);
    break;

  case 11:
    vecbanks = RS1 & 0xff;
    vecbanks_count = __builtin_popcountll(vecbanks);
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
