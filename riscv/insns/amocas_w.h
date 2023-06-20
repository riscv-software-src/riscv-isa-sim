require_extension(EXT_ZACAS);
WRITE_RD(sext32(MMU.amo_compare_and_swap<uint32_t>(RS1, RD, RS2)));
