require_extension(EXT_ZCA);
MMU.store<uint64_t>(RVC_SP + insn.rvc_sdsp_imm(), RVC_RS2);
