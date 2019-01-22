require_extension('F');
//require_extension('V');
require_fp;
WRITE_FRD(f16_to_f32(f16(MMU.load_uint16(RS1 + insn.i_imm()))));
