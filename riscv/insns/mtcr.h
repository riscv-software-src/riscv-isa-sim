switch(insn.rtype.rs2)
{
  case 0:
    set_fsr(RS1);
    break;

  case 29:
    tid = RS1;
    break;
}
