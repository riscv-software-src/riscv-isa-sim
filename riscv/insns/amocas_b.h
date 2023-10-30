require_extension('A');
require_extension(EXT_ZACAS);
require_extension(EXT_ZABHA);
WRITE_RD((sreg_t)(int8_t)(MMU.amo_compare_and_swap<uint8_t>(RS1, RD, RS2)));
