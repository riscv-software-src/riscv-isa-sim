#ifdef BODY
require_rv64;
WRITE_RD(MMU.load<uint32_t>(RS1 + I_IMM));

#endif

#define ITYPE_INSN
