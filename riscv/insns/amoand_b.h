require_extension(EXT_ZABHA);
WRITE_RD(sreg_t(MMU.amo<int8_t>(RS1, [&](int8_t lhs) { return lhs & RS2; })));
