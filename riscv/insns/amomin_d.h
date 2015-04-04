require_extension('A');
require_rv64;
sreg_t v = MMU.load_int64(RS1);
MMU.store_uint64(RS1, std::min(sreg_t(RS2),v));
WRITE_RD(v);
