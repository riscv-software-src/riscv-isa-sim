VI_VFP_BASE;
ZVBDOT_INIT(2);

switch (P.VU.vsew) {
  case 16: {
    if (P.VU.altfmt) {
      require_extension(EXT_ZVFWBDOT16BF);
      ZVBDOT_LOOP(uint16_t, uint16_t, float32_t, zvfwbdot16bf_dot_acc);
    } else {
      require(false);
    }
    break;
  }
  default: require(false);
}
