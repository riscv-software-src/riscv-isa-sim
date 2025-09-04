VI_VFP_BASE;
ZVBDOT_INIT(1);

switch (P.VU.vsew) {
  case 32: {
    // This implementation rounds intermediate products to FP32 then sums them
    // sequentially; other implementations are also valid.  If a more
    // realistic scheme (e.g. binary reduction tree, plus final accumulation)
    // becomes popular, we might change this implementation accordingly.
    require_extension(EXT_ZVFBDOT32F);
    auto macc = [](auto a, auto b, auto c) { return f32_add(c, f32_mul(a, b)); };
    ZVBDOT_GENERIC_LOOP(float32_t, float32_t, float32_t, macc);
    break;
  }
  default: require(false);
}
