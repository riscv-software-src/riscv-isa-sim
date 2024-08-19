switch (MD.getDtype()) {
    case 0: STORE_MA_REGISTER(MD, RS1 + insn.i_imm(), int8_t); break;
    case 1: STORE_MA_REGISTER(MD, RS1 + insn.i_imm(), uint8_t); break;
    case 2: STORE_MA_REGISTER(MD, RS1 + insn.i_imm(), int16_t); break;
    case 3: STORE_MA_REGISTER(MD, RS1 + insn.i_imm(), uint16_t); break;
    case 4: STORE_MA_REGISTER(MD, RS1 + insn.i_imm(), int32_t); break;
    default: STORE_MA_REGISTER(MD, RS1 + insn.i_imm(), uint32_t); break;
}