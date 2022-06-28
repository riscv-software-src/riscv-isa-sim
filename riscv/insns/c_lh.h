require_extension(EXT_ZCB);
WRITE_RVC_RS2S(MMU.load<int16_t>(RVC_RS1S + insn.rvc_lhimm()));
