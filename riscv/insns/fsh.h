require_extension(EXT_ZFHMIN);
require_fp;
MMU.store_uint16(RS1 + insn.s_imm(), FRS2.v[0]);
