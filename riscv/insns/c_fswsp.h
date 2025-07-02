require_extension(EXT_ZCF);
require_fp;
MMU.store<uint32_t>(RVC_SP + insn.rvc_swsp_imm(), RVC_FRS2.v[0]);
