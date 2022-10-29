require_extension('A');
require_rv64;
WRITE_RD(MMU.load_reserved<int64_t>(RS1));
