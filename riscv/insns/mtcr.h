switch(insn.rtype.rs2)
{
  case 0:
    require_fp;
    set_fsr(RS1);
    break;
}
