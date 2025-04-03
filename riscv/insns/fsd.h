require_extension('D');
require_fp;
MMU.store<uint64_t>(RS1 + insn.s_imm(), FRS2.v[0]);
