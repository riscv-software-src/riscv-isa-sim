require_xpr64;
RD = sext32(int32_t(RS1) >> (RS2 & 0x1F));
