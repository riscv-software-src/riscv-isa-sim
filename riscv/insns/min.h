require_either_extension(EXT_ZBPBO, EXT_ZBB);
WRITE_RD(sext_xlen(sreg_t(RS1) < sreg_t(RS2) ? RS1 : RS2));
