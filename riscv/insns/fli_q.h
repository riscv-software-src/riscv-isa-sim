require_extension('Q');
require_extension(EXT_ZFA);
require_fp;
{
  /* Indexed by rs1; high 64 bits of f128 for rs1 encodings 0b00000..0b11111.
     Low 64 bits are always zero for these constants. */
  static constexpr uint64_t bits[32] = {
    0xBFFF000000000000ull,  /* 0b00000: -1.0 */
    0x0001000000000000ull,  /* 0b00001: minimum positive normal */
    0x3FEF000000000000ull,  /* 0b00010: 1.0 * 2^-16 */
    0x3FF0000000000000ull,  /* 0b00011: 1.0 * 2^-15 */
    0x3FF7000000000000ull,  /* 0b00100: 1.0 * 2^-8  */
    0x3FF8000000000000ull,  /* 0b00101: 1.0 * 2^-7  */
    0x3FFB000000000000ull,  /* 0b00110: 1.0 * 2^-4  */
    0x3FFC000000000000ull,  /* 0b00111: 1.0 * 2^-3  */
    0x3FFD000000000000ull,  /* 0b01000: 0.25 */
    0x3FFD400000000000ull,  /* 0b01001: 0.3125 */
    0x3FFD800000000000ull,  /* 0b01010: 0.375 */
    0x3FFDC00000000000ull,  /* 0b01011: 0.4375 */
    0x3FFE000000000000ull,  /* 0b01100: 0.5 */
    0x3FFE400000000000ull,  /* 0b01101: 0.625 */
    0x3FFE800000000000ull,  /* 0b01110: 0.75 */
    0x3FFEC00000000000ull,  /* 0b01111: 0.875 */
    0x3FFF000000000000ull,  /* 0b10000: 1.0 */
    0x3FFF400000000000ull,  /* 0b10001: 1.25 */
    0x3FFF800000000000ull,  /* 0b10010: 1.5 */
    0x3FFFC00000000000ull,  /* 0b10011: 1.75 */
    0x4000000000000000ull,  /* 0b10100: 2.0 */
    0x4000400000000000ull,  /* 0b10101: 2.5 */
    0x4000800000000000ull,  /* 0b10110: 3 */
    0x4001000000000000ull,  /* 0b10111: 4 */
    0x4002000000000000ull,  /* 0b11000: 8 */
    0x4003000000000000ull,  /* 0b11001: 16 */
    0x4006000000000000ull,  /* 0b11010: 2^7 */
    0x4007000000000000ull,  /* 0b11011: 2^8 */
    0x400E000000000000ull,  /* 0b11100: 2^15 */
    0x400F000000000000ull,  /* 0b11101: 2^16 */
    0x7FFF000000000000ull,  /* 0b11110: +inf */
    defaultNaNF128UI64     /* 0b11111: canonical NaN */
  };

  static_assert(defaultNaNF128UI0 == 0, "LSBs of quad-precision NaN must be zero");

  ui128_f128 ui;
  ui.ui.v64 = bits[insn.rs1()];
  ui.ui.v0 = 0;
  WRITE_FRD(f128(ui.f));
}
