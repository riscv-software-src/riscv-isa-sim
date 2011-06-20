require_supervisor;

reg_t val;

switch(insn.rtype.rs2)
{
  case 0:
    val = sr;
    break;
  case 1:
    val = epc;
    break;
  case 2:
    val = badvaddr;
    break;
  case 3:
    val = evec;
    break;
  case 4:
    val = count;
    break;
  case 5:
    val = compare;
    break;
  case 6:
    val = cause;
    break;
  case 7:
    val = 0;
    cause &= ~(1 << (IPI_IRQ+CAUSE_IP_SHIFT));
    break;

  case 8:
    val = mmu.memsz >> PGSHIFT;
    break;

  case 9:
    val = mmu.get_ptbr();
    break;

  case 10:
    val = id;
    break;

  case 11:
    val = vecbanks;
    break;

  case 12:
    val = sim.num_cores();
    break;

  case 17:
    val = sim.get_fromhost();
    break;

  case 24:
    val = pcr_k0;
    break;
  case 25:
    val = pcr_k1;
    break;

  default:
    val = -1;
}

RD = sext_xprlen(val);
