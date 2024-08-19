switch (MD.getDtype()) {
    case 0: MA_VS_LOOP(MD, MS1, RS2, +, int8_t); break;
    case 1: MA_VS_LOOP(MD, MS1, RS2, +, uint8_t); break;
    case 2: MA_VS_LOOP(MD, MS1, RS2, +, int16_t); break;
    case 3: MA_VS_LOOP(MD, MS1, RS2, +, uint16_t); break;
    case 4: MA_VS_LOOP(MD, MS1, RS2, +, int32_t); break;
    default: MA_VS_LOOP(MD, MS1, RS2, +, uint32_t); break;
}