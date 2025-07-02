require_extension(EXT_ZCF);
require_fp;
MMU.store<uint32_t>(RVC_RS1S + insn.rvc_lw_imm(), RVC_FRS2S.v[0]);
