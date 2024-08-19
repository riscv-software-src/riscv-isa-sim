switch (MD.getDtype()) {
    case 0: MA_VV_SIMPLE_LOOP(MD, MS1, MS2, /, int8_t); break;
    case 1: MA_VV_SIMPLE_LOOP(MD, MS1, MS2, /, uint8_t); break;
    case 2: MA_VV_SIMPLE_LOOP(MD, MS1, MS2, /, int16_t); break;
    case 3: MA_VV_SIMPLE_LOOP(MD, MS1, MS2, /, uint16_t); break;
    case 4: MA_VV_SIMPLE_LOOP(MD, MS1, MS2, /, int32_t); break;
    default: MA_VV_SIMPLE_LOOP(MD, MS1, MS2, /, uint32_t); break;
}