#include "encoding.h"

#include <cassert>
#include <cstdint>
#include <limits>

enum Reg : std::uint32_t {
  ZERO = 0,
  T0   = 5,
  S0   = 8,
  S1   = 9,
};

// Bit count of the words we splice fields into, not a field width itself.
constexpr unsigned u32_bits = std::numeric_limits<std::uint32_t>::digits;

enum class BitWidth : unsigned {
  REG   = 5,
  IMM5  = 5,
  IMM12 = 12,
  IMM20 = 20,
};

constexpr std::uint32_t field_bound(BitWidth w) {
  return 1u << static_cast<unsigned>(w);
}

inline std::uint32_t
bits(std::uint32_t value, std::uint32_t hi, std::uint32_t lo) {
  assert(hi < u32_bits);
  assert(lo < u32_bits);
  assert(hi >= lo);
  auto mask = std::numeric_limits<std::uint32_t>::max() >> (u32_bits - (hi - lo + 1));
  return (value >> lo) & mask;
}

inline std::uint32_t bit(std::uint32_t value, std::uint32_t b) {
  assert(b < u32_bits);
  return (value >> b) & 1;
}

inline std::uint32_t reg_field(std::uint32_t reg) {
  assert(reg < field_bound(BitWidth::REG));
  return bits(reg, 4, 0);
}

//================================= J-type =================================//

inline std::uint32_t
j_type(std::uint32_t op, std::uint32_t rd, std::int32_t imm) {
  assert(imm >= -(1 << 20) && imm < (1 << 20));
  assert((imm & 1) == 0);

  std::uint32_t uimm = imm & 0x1fffff;
  return (bit(uimm, 20) << 31) | (bits(uimm, 10, 1) << 21) |
         (bit(uimm, 11) << 20) | (bits(uimm, 19, 12) << 12) |
         (reg_field(rd) << 7) | op;
}

inline std::uint32_t jal(std::uint32_t rd, std::int32_t imm) {
  return j_type(MATCH_JAL, rd, imm);
}

//================================= R-type =================================//

inline std::uint32_t r_type(std::uint32_t op,
                            std::uint32_t rd,
                            std::uint32_t rs1,
                            std::uint32_t rs2) {
  return (reg_field(rs2) << 20) | (reg_field(rs1) << 15) |
         (reg_field(rd) << 7) | op;
}

// Define all R-type ALU instructions using X-macro
#ifndef RTYPE_INSNS_LIST
#define RTYPE_INSNS_LIST(INSN_MACRO) \
  INSN_MACRO(and_, AND)              \
  INSN_MACRO(or_, OR)
#endif

#ifndef DEFINE_RTYPE_INSN
#define DEFINE_RTYPE_INSN(name, match)                          \
  inline std::uint32_t name(                                    \
      std::uint32_t rd, std::uint32_t rs1, std::uint32_t rs2) { \
    return r_type(MATCH_##match, rd, rs1, rs2);                 \
  }
#endif

RTYPE_INSNS_LIST(DEFINE_RTYPE_INSN)
#undef DEFINE_RTYPE_INSN
#undef RTYPE_INSNS_LIST

//================================= S-type =================================//

inline std::uint32_t s_type(std::uint32_t op,
                            std::uint32_t rs1,
                            std::uint32_t rs2,
                            std::int32_t imm) {
  assert(imm >= -2048 && imm <= 2047);

  std::uint32_t uimm = imm & 0xfff;
  return (bits(uimm, 11, 5) << 25) | (reg_field(rs2) << 20) |
         (reg_field(rs1) << 15) | (bits(uimm, 4, 0) << 7) | op;
}

// Define all S-type store instructions using X-macro
#ifndef STORE_INSNS_LIST
#define STORE_INSNS_LIST(INSN_MACRO) \
  INSN_MACRO(sw, SW)                 \
  INSN_MACRO(sd, SD)                 \
  INSN_MACRO(sh, SH)                 \
  INSN_MACRO(sb, SB)                 \
  INSN_MACRO(fsw, FSW)               \
  INSN_MACRO(fsd, FSD)
#endif

#ifndef DEFINE_STORE_INSN
#define DEFINE_STORE_INSN(name, match)                             \
  inline std::uint32_t name(                                       \
      std::uint32_t rs2, std::uint32_t rs1, std::int32_t offset) { \
    return s_type(MATCH_##match, rs1, rs2, offset);                \
  }
#endif

STORE_INSNS_LIST(DEFINE_STORE_INSN)
#undef DEFINE_STORE_INSN
#undef STORE_INSNS_LIST

//================================= I-type =================================//

inline std::uint32_t i_type(std::uint32_t op,
                            std::uint32_t rd,
                            std::uint32_t rs1,
                            std::int32_t imm) {
  assert(imm >= -2048 && imm <= 2047);

  std::uint32_t uimm = imm & 0xfff;
  return (bits(uimm, 11, 0) << 20) | (reg_field(rs1) << 15) |
         (reg_field(rd) << 7) | op;
}

// Define all I-type load instructions using X-macro
#ifndef LOAD_INSNS_LIST
#define LOAD_INSNS_LIST(INSN_MACRO) \
  INSN_MACRO(ld, LD)                \
  INSN_MACRO(lw, LW)                \
  INSN_MACRO(lh, LH)                \
  INSN_MACRO(lb, LB)                \
  INSN_MACRO(flw, FLW)              \
  INSN_MACRO(fld, FLD)
#endif

#ifndef DEFINE_LOAD_INSN
#define DEFINE_LOAD_INSN(name, match)                             \
  inline std::uint32_t name(                                      \
      std::uint32_t rd, std::uint32_t rs1, std::int32_t offset) { \
    return i_type(MATCH_##match, rd, rs1, offset);                \
  }
#endif

LOAD_INSNS_LIST(DEFINE_LOAD_INSN)
#undef DEFINE_LOAD_INSN
#undef LOAD_INSNS_LIST

// Define all I-type immediate ALU instructions using X-macro
#ifndef IMM_ALU_INSNS_LIST
#define IMM_ALU_INSNS_LIST(INSN_MACRO) \
  INSN_MACRO(addi, ADDI)               \
  INSN_MACRO(andi, ANDI)               \
  INSN_MACRO(ori, ORI)
#endif

#ifndef DEFINE_IMM_ALU_INSN
#define DEFINE_IMM_ALU_INSN(name, match)                       \
  inline std::uint32_t name(                                   \
      std::uint32_t rd, std::uint32_t rs1, std::int32_t imm) { \
    return i_type(MATCH_##match, rd, rs1, imm);                \
  }
#endif

IMM_ALU_INSNS_LIST(DEFINE_IMM_ALU_INSN)
#undef DEFINE_IMM_ALU_INSN
#undef IMM_ALU_INSNS_LIST

//================================= U-type =================================//

inline std::uint32_t
u_type(std::uint32_t op, std::uint32_t rd, std::uint32_t imm) {
  assert(imm < field_bound(BitWidth::IMM20));
  return (bits(imm, 19, 0) << 12) | (reg_field(rd) << 7) | op;
}

inline std::uint32_t lui(std::uint32_t rd, std::uint32_t imm) {
  return u_type(MATCH_LUI, rd, imm);
}

//================================== CSRs ==================================//

inline std::uint32_t csr_rtype(std::uint32_t op,
                               std::uint32_t rd,
                               std::uint32_t rs1,
                               std::uint32_t csr) {
  assert(csr < field_bound(BitWidth::IMM12));
  return (bits(csr, 11, 0) << 20) | (reg_field(rs1) << 15) |
         (reg_field(rd) << 7) | op;
}

inline std::uint32_t csr_itype(std::uint32_t op,
                               std::uint32_t rd,
                               std::uint32_t imm,
                               std::uint32_t csr) {
  assert(imm < field_bound(BitWidth::IMM5));
  assert(csr < field_bound(BitWidth::IMM12));
  return (bits(csr, 11, 0) << 20) | (bits(imm, 4, 0) << 15) |
         (reg_field(rd) << 7) | op;
}

// Define CSR register-based instructions using X-macro
#ifndef CSR_REG_INSNS_LIST
#define CSR_REG_INSNS_LIST(INSN_MACRO) \
  INSN_MACRO(csrrs, CSRRS)             \
  INSN_MACRO(csrrc, CSRRC)
#endif

#ifndef DEFINE_CSR_REG_INSN
#define DEFINE_CSR_REG_INSN(name, match)                        \
  inline std::uint32_t name(                                    \
      std::uint32_t rd, std::uint32_t rs1, std::uint32_t csr) { \
    return csr_rtype(MATCH_##match, rd, rs1, csr);              \
  }
#endif

CSR_REG_INSNS_LIST(DEFINE_CSR_REG_INSN)
#undef DEFINE_CSR_REG_INSN
#undef CSR_REG_INSNS_LIST

inline std::uint32_t
csrrw(std::uint32_t rd, std::uint32_t rs1, std::uint32_t csr) {
  return csr_rtype(MATCH_CSRRW, rd, rs1, csr);
}

inline std::uint32_t csrw(std::uint32_t rs1, std::uint32_t csr) {
  return csrrw(ZERO, rs1, csr);
}

inline std::uint32_t csrr(std::uint32_t rd, std::uint32_t csr) {
  return csrrs(rd, ZERO, csr);
}

// Define CSR immediate-based instructions using X-macro
#ifndef CSR_IMM_INSNS_LIST
#define CSR_IMM_INSNS_LIST(INSN_MACRO) \
  INSN_MACRO(csrrsi, CSRRSI)           \
  INSN_MACRO(csrrci, CSRRCI)
#endif

#ifndef DEFINE_CSR_IMM_INSN
#define DEFINE_CSR_IMM_INSN(name, match)                        \
  inline std::uint32_t name(                                    \
      std::uint32_t rd, std::uint32_t imm, std::uint32_t csr) { \
    return csr_itype(MATCH_##match, rd, imm, csr);              \
  }
#endif

CSR_IMM_INSNS_LIST(DEFINE_CSR_IMM_INSN)
#undef DEFINE_CSR_IMM_INSN
#undef CSR_IMM_INSNS_LIST

//================================= MISC =================================//

inline std::uint32_t ebreak(void) {
  return MATCH_EBREAK;
}
