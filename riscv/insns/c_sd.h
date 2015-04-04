require_extension('C');
require_rv64;
MMU.store_uint64(RVC_RS1S + insn.rvc_ld_imm(), RVC_RS2S);
