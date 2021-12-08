require_extension(EXT_ZFHMIN);
require_fp;
WRITE_FRD(f16(MMU.load_uint16(RS1 + insn.i_imm())));
