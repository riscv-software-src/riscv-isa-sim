require_extension(EXT_ZCA);
WRITE_RVC_RS2S(MMU.load<int64_t>(RVC_RS1S + insn.rvc_ld_imm()));
