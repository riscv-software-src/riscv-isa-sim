require_extension('Q');
require_fp;
WRITE_FRD(MMU.load_float128(RS1 + insn.i_imm()));
