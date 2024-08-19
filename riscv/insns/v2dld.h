switch (MD.getDtype()) {
    case 0: LOAD_MA_REGISTER(MD, RS1 + insn.i_imm(), int8_t); break;
    case 1: LOAD_MA_REGISTER(MD, RS1 + insn.i_imm(), uint8_t); break;
    case 2: LOAD_MA_REGISTER(MD, RS1 + insn.i_imm(), int16_t); break;
    case 3: LOAD_MA_REGISTER(MD, RS1 + insn.i_imm(), uint16_t); break;
    case 4: LOAD_MA_REGISTER(MD, RS1 + insn.i_imm(), int32_t); break;
    default: LOAD_MA_REGISTER(MD, RS1 + insn.i_imm(), uint32_t); break;
}