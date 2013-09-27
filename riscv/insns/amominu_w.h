uint32_t v = MMU.load_int32(RS1);
MMU.store_uint32(RS1, std::min(uint32_t(RS2),v));
WRITE_RD((int32_t)v);
