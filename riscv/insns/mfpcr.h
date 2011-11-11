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
    val = mmu.get_ptbr();
    break;

  case 10:
    val = id;
    break;

  case 12:
    val = pcr_k0;
    break;
  case 13:
    val = pcr_k1;
    break;

  case 17:
    val = sim.get_fromhost();
    break;

  case 18:
    val = vecbanks;
    break;

  default:
    val = -1;
}

RD = sext_xprlen(val);
