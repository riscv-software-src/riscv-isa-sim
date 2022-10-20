require_extension('C');
require_extension('D');
require_fp;
MMU.store<uint64_t>(RVC_RS1S + insn.rvc_ld_imm(), RVC_FRS2S.v[0]);
