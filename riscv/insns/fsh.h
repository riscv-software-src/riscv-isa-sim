require_extension('F');
//require_extension('V');
require_fp;
MMU.store_uint16(RS1 + insn.s_imm(), f32_to_f16(f32(unboxF32(FRS2))).v);
