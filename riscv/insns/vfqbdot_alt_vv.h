VI_VFP_BASE;
ZVBDOT_INIT(4);

#define COMMA ,

switch (P.VU.vsew) {
  case 8: {
    require_extension(EXT_ZVFQBDOT8F);
    if (P.VU.altfmt) {
      ZVBDOT_LOOP(uint8_t, uint8_t, float32_t, zvfqbdot8f_dot_acc<ofp8_e5m2 COMMA ofp8_e5m2>);
    } else {
      ZVBDOT_LOOP(uint8_t, uint8_t, float32_t, zvfqbdot8f_dot_acc<ofp8_e4m3 COMMA ofp8_e5m2>);
    }
    break;
  }
  default: require(false);
}
