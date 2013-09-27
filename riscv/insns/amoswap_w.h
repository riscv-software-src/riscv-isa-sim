reg_t v = MMU.load_int32(RS1);
MMU.store_uint32(RS1, RS2);
WRITE_RD(v);
