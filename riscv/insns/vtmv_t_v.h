ZVT_BASE;
ZVT_CHECK_ALIGN_RS2;

#define LOOP(type) for (size_t i = 0; i < P.VU.tn(); i++) P.VU.tss_elt<type>(RS1, i) = P.VU.elt<type>(insn.rs2(), i)

switch (P.VU.vsew) {
  case e8: require_extension(EXT_ZVT8E); LOOP(uint8_t); break;
  case e16: require_extension(EXT_ZVT16E); LOOP(uint16_t); break;
  case e32: LOOP(uint32_t); break;
  case e64: require_extension(EXT_ZVT64E); LOOP(uint64_t); break;
  default: abort();
}

ZVT_END;
