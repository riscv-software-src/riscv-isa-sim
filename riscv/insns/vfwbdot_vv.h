VI_VFP_BASE;
ZVBDOT_INIT(2);

switch (P.VU.vsew) {
  case 16: {
    if (P.VU.altfmt) {
      // TODO replace with bulk-norm routine and invoke ZVBDOT_LOOP instead of ZVBDOT_GENERIC_LOOP
      require_extension(EXT_ZVFWBDOT16BF);
      auto macc = [](auto a, auto b, auto c) { return f32_add(c, f32_mul(bf16_to_f32(a), bf16_to_f32(b))); };
      ZVBDOT_GENERIC_LOOP(bfloat16_t, bfloat16_t, float32_t, macc);
    } else {
      require(false);
    }
    break;
  }
  default: require(false);
}
