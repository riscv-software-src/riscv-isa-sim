require64;
reg_t v = mmu.load_uint64(RS1);
mmu.store_uint64(RS1, std::max(RS2,v));
RDR = v;
