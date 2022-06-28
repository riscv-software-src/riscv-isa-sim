require_extension(EXT_ZCD);
require_fp;
MMU.store<uint64_t>(RVC_SP + insn.rvc_sdsp_imm(), RVC_FRS2.v[0]);
