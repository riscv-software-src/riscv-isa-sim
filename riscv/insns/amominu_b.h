require_extension(EXT_ZABHA);
WRITE_RD(sreg_t(MMU.amo<int8_t>(RS1, [&](uint8_t lhs) { return std::min(lhs, uint8_t(RS2)); })));
