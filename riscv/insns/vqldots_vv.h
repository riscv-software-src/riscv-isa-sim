ZVLDOT_INIT(4);

switch (P.VU.vsew) {
  case 8: {
    require_extension(EXT_ZVQLDOT8I);
    if (P.VU.altfmt) {
      ZVLDOT_SIMPLE_LOOP(int8_t, int8_t, uint32_t);
    } else {
      ZVLDOT_SIMPLE_LOOP(uint8_t, int8_t, uint32_t);
    }
    break;
  }
  case 16: {
    require_extension(EXT_ZVQLDOT16I);
    if (P.VU.altfmt) {
      ZVLDOT_SIMPLE_LOOP(int16_t, int16_t, uint64_t);
    } else {
      ZVLDOT_SIMPLE_LOOP(uint16_t, int16_t, uint64_t);
    }
    break;
  }
  default: require(false);
}
