require_extension('C');
MMU.store<uint32_t>(RVC_RS1S + insn.rvc_lw_imm(), RVC_RS2S);
