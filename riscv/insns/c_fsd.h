require_extension(EXT_ZCD);
require_fp;
MMU.store<uint64_t>(RVC_RS1S + insn.rvc_ld_imm(), RVC_FRS2S.v[0]);
