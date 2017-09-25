require_extension('F');
require_fp;
MMU.store_uint32(RS1 + insn.s_imm(), FRS2.v[0]);
