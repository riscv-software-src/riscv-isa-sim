require_extension(EXT_ZABHA);
WRITE_RD(sreg_t(MMU.amo<int16_t>(RS1, [&](int16_t lhs) { return std::max(lhs, int16_t(RS2)); })));
