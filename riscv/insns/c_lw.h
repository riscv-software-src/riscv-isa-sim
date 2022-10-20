require_extension('C');
WRITE_RVC_RS2S(MMU.load<int32_t>(RVC_RS1S + insn.rvc_lw_imm()));
