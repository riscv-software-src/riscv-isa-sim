require_extension(EXT_ZFHMIN);
require_fp;
WRITE_FRD(f16(MMU.load<uint16_t>(RS1 + insn.i_imm())));
