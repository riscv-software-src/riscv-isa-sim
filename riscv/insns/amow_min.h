int32_t v = mmu.load_int32(RB);
mmu.store_uint32(RB, std::min(int32_t(RA),v));
RC = v;
