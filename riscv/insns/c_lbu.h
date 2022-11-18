require_extension(EXT_ZCB);
WRITE_RVC_RS2S(MMU.load<uint8_t>(RVC_RS1S + insn.rvc_lbimm()));
