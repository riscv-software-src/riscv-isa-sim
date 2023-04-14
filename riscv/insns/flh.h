require_either_extensions(EXT_ZFHMIN, EXT_ZFBFMIN, EXT_ZVFBFMIN);
require_fp;
WRITE_FRD(f16(MMU.load<uint16_t>(RS1 + insn.i_imm())));
