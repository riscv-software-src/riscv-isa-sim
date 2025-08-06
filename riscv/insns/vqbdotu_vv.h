ZVBDOT_INIT(4);

switch (P.VU.vsew) {
  case 8: {
    require_extension(EXT_ZVQBDOT8I);
    if (P.VU.altfmt) {
      ZVBDOT_SIMPLE_LOOP(int8_t, uint8_t, uint32_t);
    } else {
      ZVBDOT_SIMPLE_LOOP(uint8_t, uint8_t, uint32_t);
    }
    break;
  }
  case 16: {
    require_extension(EXT_ZVQBDOT16I);
    if (P.VU.altfmt) {
      ZVBDOT_SIMPLE_LOOP(int16_t, uint16_t, uint64_t);
    } else {
      ZVBDOT_SIMPLE_LOOP(uint16_t, uint16_t, uint64_t);
    }
    break;
  }
  default: require(false);
}
