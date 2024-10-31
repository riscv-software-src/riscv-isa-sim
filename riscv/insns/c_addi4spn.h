#ifdef BODY
require_extension(EXT_ZCA);
require(insn.ciwtype.rvc_addi4spn_imm != 0);
WRITE_RVC_RS2S(sext_xlen(RVC_SP + insn.ciwtype.rvc_addi4spn_imm));

#endif

#define CIWTYPE_INSN
