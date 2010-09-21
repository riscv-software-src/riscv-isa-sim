int32_t v = mmu.load_int32(RS1);
mmu.store_uint32(RS1, std::min(int32_t(RS2),v));
RDR = v;
