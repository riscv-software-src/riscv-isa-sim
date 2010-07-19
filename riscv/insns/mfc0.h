require_supervisor;

switch(insn.rtype.rs)
{
  case 0:
    RT = sext32(sr);
    break;
  case 1:
    RT = sext32(epc);
    break;
  case 2:
    RT = sext32(badvaddr);
    break;
  case 3:
    RT = sext32(ebase);
    break;
  default:
    RT = -1;
}
