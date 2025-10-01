VI_VFP_BASE;
ZVLDOT_INIT(4);

#define COMMA ,

switch (P.VU.vsew) {
  case 8: {
    require_extension(EXT_ZVFQLDOT8F);
    if (P.VU.altfmt) {
      ZVLDOT_LOOP(uint8_t, uint8_t, float32_t, zvfqbdot8f_dot_acc<ofp8_e5m2 COMMA ofp8_e4m3>);
    } else {
      ZVLDOT_LOOP(uint8_t, uint8_t, float32_t, zvfqbdot8f_dot_acc<ofp8_e4m3 COMMA ofp8_e4m3>);
    }
    break;
  }
  default: require(false);
}
