require_extension(EXT_ZCA);
WRITE_RVC_RS2S(MMU.load<int32_t>(RVC_RS1S + insn.rvc_lw_imm()));
