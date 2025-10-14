// See LICENSE for license details.
#ifndef _RISCV_VECTOR_UNIT_H
#define _RISCV_VECTOR_UNIT_H

#include <array>
#include <cstdint>

#include "decode.h"
#include "csrs.h"

class processor_t;

enum VRM{
  RNU = 0,
  RNE,
  RDN,
  ROD,
  INVALID_RM
};

template<uint64_t N>
struct type_usew_t;

template<>
struct type_usew_t<8>
{
  using type=uint8_t;
};

template<>
struct type_usew_t<16>
{
  using type=uint16_t;
};

template<>
struct type_usew_t<32>
{
  using type=uint32_t;
};

template<>
struct type_usew_t<64>
{
  using type=uint64_t;
};

template<uint64_t N>
struct type_sew_t;

template<>
struct type_sew_t<8>
{
  using type=int8_t;
};

template<>
struct type_sew_t<16>
{
  using type=int16_t;
};

template<>
struct type_sew_t<32>
{
  using type=int32_t;
};

template<>
struct type_sew_t<64>
{
  using type=int64_t;
};

// Element Group of 4 32 bits elements (128b total).
using EGU32x4_t = std::array<uint32_t, 4>;

// Element Group of 8 32 bits elements (256b total).
using EGU32x8_t = std::array<uint32_t, 8>;

// Element Group of 4 64 bits elements (256b total).
using EGU64x4_t = std::array<uint64_t, 4>;

// Element Group of 16 8 bits elements (128b total).
using EGU8x16_t = std::array<uint8_t, 16>;

class vectorUnit_t
{
public:
  processor_t* p = nullptr;
  void *reg_file = nullptr;
  int setvl_count = 0;
  reg_t vlmax = 0;
  reg_t vlenb = 0;
  csr_t_p vxsat = 0;
  vector_csr_t_p vxrm = 0, vstart = 0, vl = 0, vtype = 0;
  reg_t vma = 0, vta = 0;
  reg_t vsew = 0;
  float vflmul = 0;
  reg_t altfmt = 0;
  reg_t ELEN = 0, VLEN = 0;
  bool vill = false;
  bool vstart_alu = false;

  // vector element for various SEW
  template<typename T> T& elt(reg_t vReg, reg_t n, bool is_write = false) {
    assert(vsew != 0);
    assert((VLEN >> 3)/sizeof(T) > 0);
    reg_t elts_per_reg = (VLEN >> 3) / (sizeof(T));
    vReg += n / elts_per_reg;
    n = n % elts_per_reg;
#ifdef WORDS_BIGENDIAN
    // "V" spec 0.7.1 requires lower indices to map to lower significant
    // bits when changing SEW, thus we need to index from the end on BE.
    n ^= elts_per_reg - 1;
#endif
    if (is_write)
      log_elt_write_if_needed(vReg);

    T *regStart = (T*)((char*)reg_file + vReg * (VLEN >> 3));
    return regStart[n];
  }

  // vector element group access, where EG is a std::array<T, N>.
  // The logic differences between 'elt()' and 'elt_group()' come from
  // the fact that, while 'elt()' requires that the element is fully
  // contained in a single vector register, the element group may span
  // multiple registers in a single register group (LMUL>1).
  //
  // Notes:
  // - We do NOT check that a single element - i.e., the T in the element
  //   group type std::array<T, N> - fits within a single register, or that
  //   T is smaller or equal to VSEW. Implementations of the instructions
  //   sometimes use a different T than what the specification suggests.
  //   Instructon implementations should 'require()' what the specification
  //   dictates.
  // - We do NOT check that 'vReg' is a valid register group, or that
  //   'n+1' element groups fit in the register group 'vReg'. It is
  //   the responsibility of the caller to validate those preconditions.

  template<typename EG> EG&
  elt_group(reg_t vReg, reg_t n, bool is_write = false) {
#ifdef WORDS_BIGENDIAN
    fputs("vectorUnit_t::elt_group is not compatible with WORDS_BIGENDIAN setup.\n",
            stderr);
    abort();
#endif
    using T = typename EG::value_type;
    constexpr std::size_t N = std::tuple_size<EG>::value;
    assert(N > 0);

    assert(vsew != 0);
    constexpr reg_t elt_group_size = N * sizeof(T);
    const reg_t reg_group_size = (VLEN >> 3) * vflmul;
    assert(((n + 1) * elt_group_size) <= reg_group_size);

    const reg_t start_byte = n * elt_group_size;
    const reg_t bytes_per_reg = VLEN >> 3;

    // Inclusive first/last register indices.
    const reg_t reg_first = vReg + start_byte / bytes_per_reg;
    const reg_t reg_last = vReg + (start_byte + elt_group_size - 1) / bytes_per_reg;

    // Element groups per register groups
    for (reg_t vidx = reg_first; vidx <= reg_last; ++vidx)
      if (is_write)
        log_elt_write_if_needed(vidx);

    return *(EG*)((char*)reg_file + vReg * (VLEN >> 3) + start_byte);
  }

  bool mask_elt(reg_t vReg, reg_t n)
  {
    return (elt<uint8_t>(vReg, n / 8) >> (n % 8)) & 1;
  }

  void set_mask_elt(reg_t vReg, reg_t n, bool value)
  {
    auto& e = elt<uint8_t>(vReg, n / 8, true);
    e = (e & ~(1U << (n % 8))) | (value << (n % 8));
  }

private:

  void log_elt_write_if_needed(reg_t vReg) const;

public:

  void reset();

  vectorUnit_t() {}

  ~vectorUnit_t() {
    free(reg_file);
    reg_file = 0;
  }

  reg_t set_vl(int rd, int rs1, reg_t reqVL, reg_t newType);

  reg_t get_vlen() { return VLEN; }
  reg_t get_elen() { return ELEN; }
  reg_t get_slen() { return VLEN; }

  VRM get_vround_mode() {
    return (VRM)(vxrm->read());
  }
};
#endif
