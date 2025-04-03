require_extension('A');
WRITE_RD(sext32(MMU.amo<uint32_t>(RS1, [&](uint32_t UNUSED lhs) { return RS2; })));
