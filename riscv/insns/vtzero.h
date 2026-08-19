ZVT_MTD(16);

ZVT_MN_LOOP
({
  switch (tew) {
    case 8: P.VU.mt_elt<uint8_t>(td, m, n) = 0; break;
    case 16: P.VU.mt_elt<uint16_t>(td, m, n) = 0; break;
    case 32: P.VU.mt_elt<uint32_t>(td, m, n) = 0; break;
    case 64: P.VU.mt_elt<uint64_t>(td, m, n) = 0; break;
    default: abort();
  }
})

ZVT_END;
