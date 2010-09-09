switch(insn.rtype.rb)
{
  case 0:
    set_fsr(RA);
    break;

  case 29:
    tid = RA;
    break;
}
