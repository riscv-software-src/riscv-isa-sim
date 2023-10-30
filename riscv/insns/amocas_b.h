require_extension(EXT_ZACAS);
require_extension(EXT_ZABHA);
WRITE_RD(sreg_t(MMU.amo_compare_and_swap<int8_t>(RS1, RD, RS2)));
