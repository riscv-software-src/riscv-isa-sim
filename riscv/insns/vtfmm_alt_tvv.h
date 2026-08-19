ZVT_MTD(8);
ZVT_CHECK_ALIGN_RS1;
ZVT_CHECK_ALIGN_RS2;
ZVT_CHECK_RS1_EMUL;
ZVT_CHECK_RS2_EMUL;
require_fp;

softfloat_roundingMode = VFP_RM;

if (P.VU.vsew == 16 && P.VU.widen == 2 && P.VU.altfmt) {
  require_extension(EXT_ZVTBF16FMM);

  if (P.VU.tk) {
    const size_t kmax = 2;
    auto cfg = DotConfig(kmax, 1);
    ZVT_MN_LOOP
    ({
      auto a = P.VU.mt_vector<float16_t>(insn.rs2(), m);
      auto b = P.VU.mt_vector<float16_t>(insn.rs1(), n);
      auto& res = P.VU.mt_elt<float32_t>(td, m, n);

      bf16_t in_a[kmax];
      bf16_t in_b[kmax];

      for (size_t ki = 0; ki < P.VU.tk; ki++) {
        in_a[ki].n = a[ki].v;
        in_b[ki].n = b[ki].v;
      }
      auto out = bulk_norm_dot_bf16(cfg, in_a, in_b);

      softfloat_exceptionFlags |= out.flags;
      res = f32_add_odd(res, f32(out.out));
      softfloat_exceptionFlags &= softfloat_flag_invalid | softfloat_flag_overflow;
    })
  }
} else if (P.VU.vsew == 8 && P.VU.widen == 4 && !P.VU.altfmt) {
  require_extension(EXT_ZVTOFP8FMM);

  if (P.VU.tk) {
    const size_t kmax = 4;
    auto cfg = DotConfig(kmax, 2);
    ZVT_MN_LOOP
    ({
      auto a = P.VU.mt_vector<float8_t>(insn.rs2(), m);
      auto b = P.VU.mt_vector<float8_t>(insn.rs1(), n);
      auto& res = P.VU.mt_elt<float32_t>(td, m, n);

      ofp8_e5m2 in_a[kmax];
      ofp8_e4m3 in_b[kmax];

      for (size_t ki = 0; ki < P.VU.tk; ki++) {
        in_a[ki].n = a[ki].v;
        in_b[ki].n = b[ki].v;
      }
      auto out = bulk_norm_dot_ofp8(cfg, in_a, in_b);

      softfloat_exceptionFlags |= out.flags;
      res = f32_add_odd(res, f32(out.out));
      softfloat_exceptionFlags &= softfloat_flag_invalid | softfloat_flag_overflow;
    })
  }
} else if (P.VU.vsew == 8 && P.VU.widen == 4 && P.VU.altfmt) {
  require_extension(EXT_ZVTOFP8FMM);

  if (P.VU.tk) {
    const size_t kmax = 4;
    auto cfg = DotConfig(kmax, 2);
    ZVT_MN_LOOP
    ({
      auto a = P.VU.mt_vector<float8_t>(insn.rs2(), m);
      auto b = P.VU.mt_vector<float8_t>(insn.rs1(), n);
      auto& res = P.VU.mt_elt<float32_t>(td, m, n);

      ofp8_e5m2 in_a[kmax];
      ofp8_e5m2 in_b[kmax];

      for (size_t ki = 0; ki < P.VU.tk; ki++) {
        in_a[ki].n = a[ki].v;
        in_b[ki].n = b[ki].v;
      }
      auto out = bulk_norm_dot_ofp8(cfg, in_a, in_b);

      softfloat_exceptionFlags |= out.flags;
      res = f32_add_odd(res, f32(out.out));
      softfloat_exceptionFlags &= softfloat_flag_invalid | softfloat_flag_overflow;
    })
  }
} else {
  require(false);
}

set_fp_exceptions;
