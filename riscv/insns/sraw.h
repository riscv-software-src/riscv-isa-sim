require_xpr64;
RD = sext32(sreg_t(RS1) >> (RS2 & 0x1F));
