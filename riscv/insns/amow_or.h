reg_t v = mmu.load_int32(RS1);
mmu.store_uint32(RS1, RS2 | v);
RDR = v;
