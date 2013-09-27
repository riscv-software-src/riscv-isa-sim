require_xpr64;
reg_t v = MMU.load_uint64(RS1);
MMU.store_uint64(RS1, RS2 | v);
WRITE_RD(v);
