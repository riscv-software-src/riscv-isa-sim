require_either_extensions(EXT_ZFHMIN, EXT_ZFBFMIN, EXT_ZVFBFMIN);
require_fp;
MMU.store<uint16_t>(RS1 + insn.s_imm(), FRS2.v[0]);
