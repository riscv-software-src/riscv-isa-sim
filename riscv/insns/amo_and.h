require64;
reg_t v = mmu.load_uint64(RB);
mmu.store_uint64(RB, RA & v);
RC = v;
