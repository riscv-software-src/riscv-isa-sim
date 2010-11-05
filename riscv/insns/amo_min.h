require64;
sreg_t v = mmu.load_int64(RS1);
mmu.store_uint64(RS1, std::min(sreg_t(RS2),v));
RD = v;
