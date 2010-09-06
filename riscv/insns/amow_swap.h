reg_t v = mmu.load_int32(RB);
mmu.store_uint32(RB, RA);
RC = v;
