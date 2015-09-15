require_extension('F');
require_fp;
WRITE_FRD(MMU.load_uint32(RS1 + insn.i_imm()));
