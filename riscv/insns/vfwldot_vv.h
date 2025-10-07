VI_VFP_BASE;
ZVLDOT_INIT(2);

switch (P.VU.vsew) {
  case 16: {
    if (P.VU.altfmt) {
      // Although this implementation in IEEE 754 arithmetic is valid, most
      // implementations will bulk-normalize on a VLEN-bit granule, then use
      // f32_add_bulknorm_odd for the final steps (possibly in a tree).
      // If a consensus emerges, we might change this implementation.
      require_extension(EXT_ZVFWLDOT16BF);
      auto macc = [](auto a, auto b, auto c) { return f32_add_bulknorm_odd(c, f32_mul(bf16_to_f32(a), bf16_to_f32(b))); };
      ZVLDOT_GENERIC_LOOP(bfloat16_t, bfloat16_t, float32_t, macc);
    } else {
      require(false);
    }
    break;
  }
  default: require(false);
}
