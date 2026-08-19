ZVT_MTD(4);
ZVT_CHECK_ALIGN_RS1;
ZVT_CHECK_ALIGN_RS2;
ZVT_CHECK_RS1_EMUL;
ZVT_CHECK_RS2_EMUL;

if (P.VU.vsew == 8 && P.VU.widen == 4 && !P.VU.altfmt) {
  require_extension(EXT_ZVTI8I32MM);

  ZVT_MN_LOOP
  ({
    auto a = P.VU.mt_vector<uint8_t>(insn.rs2(), m);
    auto b = P.VU.mt_vector<uint8_t>(insn.rs1(), n);
  
    P.VU.mt_elt<int32_t>(td, m, n) += dot_product<int32_t>(a.data(), b.data(), P.VU.tk);
  })
} else if (P.VU.vsew == 8 && P.VU.widen == 4 && P.VU.altfmt) {
  require_extension(EXT_ZVTI8I32MM);

  ZVT_MN_LOOP
  ({
    auto a = P.VU.mt_vector<uint8_t>(insn.rs2(), m);
    auto b = P.VU.mt_vector<int8_t>(insn.rs1(), n);
  
    P.VU.mt_elt<int32_t>(td, m, n) += dot_product<int32_t>(a.data(), b.data(), P.VU.tk);
  })
} else {
  require(false);
}
