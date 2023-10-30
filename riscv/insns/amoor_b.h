require_extension('A');
require_extension(EXT_ZABHA);
WRITE_RD((sreg_t)(int8_t)(MMU.amo<uint8_t>(RS1, [&](uint8_t lhs) { return lhs | RS2; })));
