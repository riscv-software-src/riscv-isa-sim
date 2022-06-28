require_extension(EXT_ZCB);
MMU.store<uint16_t>(RVC_RS1S + insn.rvc_lhimm(), RVC_RS2S);
