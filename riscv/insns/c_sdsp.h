require_extension('C');
require_rv64;
MMU.store_uint64(RVC_SP + insn.rvc_ldsp_imm(), RVC_RS2);
