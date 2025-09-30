VI_VFP_BASE;
ZVLDOT_INIT(2);

switch (P.VU.vsew) {
  case 16: {
    if (P.VU.altfmt) {
      require_extension(EXT_ZVFWLDOT16BF);
      ZVLDOT_LOOP(uint16_t, uint16_t, float32_t, zvfwbdot16bf_dot_acc);
    } else {
      require(false);
    }
    break;
  }
  default: require(false);
}
