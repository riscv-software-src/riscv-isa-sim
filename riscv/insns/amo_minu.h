require64;
reg_t v = mmu.load_uint64(RS1);
mmu.store_uint64(RS1, std::min(RS2,v));
RDR = v;
