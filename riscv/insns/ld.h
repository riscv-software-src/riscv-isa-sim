#ifdef BODY
require((xlen == 64) || p->extension_enabled(EXT_ZILSD));

if (xlen == 32) {
  WRITE_RD_PAIR(MMU.load<int64_t>(RS1 + I_IMM));
} else {
  WRITE_RD(MMU.load<int64_t>(RS1 + I_IMM));
}


#endif

#define ITYPE_INSN
