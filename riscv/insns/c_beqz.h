#ifdef BODY
require_extension(EXT_ZCA);
if (RVC_RS1S == 0)
  set_pc(pc + RVC_B_IMM);

#endif
#define CBBTYPE_INSN
