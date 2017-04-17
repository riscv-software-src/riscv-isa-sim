require_extension('C');
require_extension('D');
require_fp;
MMU.store_uint64(RVC_RS1S + insn.rvc_ld_imm(), RVC_FRS2S.v);
