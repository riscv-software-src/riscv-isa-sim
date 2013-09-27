require_xpr64;
sreg_t v = MMU.load_int64(RS1);
MMU.store_uint64(RS1, std::max(sreg_t(RS2),v));
WRITE_RD(v);
