#ifdef BODY
require((xlen == 64) || p->extension_enabled(EXT_ZILSD));

if (xlen == 32) {
  MMU.store<uint64_t>(RS1 + S_IMM, RS2_PAIR);
} else {
  MMU.store<uint64_t>(RS1 + S_IMM, RS2);
}

#endif

#define STYPE_INSN
