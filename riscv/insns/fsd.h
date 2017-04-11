require_extension('D');
require_fp;
MMU.store_uint64(RS1 + insn.s_imm(), FRS2.v);
