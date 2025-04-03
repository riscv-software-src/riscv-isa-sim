// See LICENSE for license details.

#ifndef _RISCV_BYTEORDER_H
#define _RISCV_BYTEORDER_H

#include <stdint.h>
#include <arpa/inet.h>

static inline uint8_t swap(uint8_t n) { return n; }
static inline uint16_t swap(uint16_t n) { return (n >> 8) | (n << 8); }
static inline uint32_t swap(uint32_t n) { return (swap(uint16_t(n)) << 16) | swap(uint16_t(n >> 16)); }
static inline uint64_t swap(uint64_t n) { return (uint64_t(swap(uint32_t(n))) << 32) | swap(uint32_t(n >> 32)); }
static inline int8_t swap(int8_t n) { return n; }
static inline int16_t swap(int16_t n) { return int16_t(swap(uint16_t(n))); }
static inline int32_t swap(int32_t n) { return int32_t(swap(uint32_t(n))); }
static inline int64_t swap(int64_t n) { return int64_t(swap(uint64_t(n))); }

#ifdef HAVE_INT128
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
static inline uint128_t swap(uint128_t n) { return (uint128_t(swap(uint64_t(n))) << 64) | swap(uint64_t(n >> 64)); }
static inline int128_t swap(int128_t n) { return int128_t(swap(uint128_t(n))); }
#endif

static inline bool is_be() { return htonl(1) == 1; }
template<typename T> static inline T from_be(T n) { return is_be() ? n : swap(n); }
template<typename T> static inline T to_be(T n) { return from_be(n); }
template<typename T> static inline T from_le(T n) { return is_be() ? swap(n) : n; }
template<typename T> static inline T to_le(T n) { return from_le(n); }

// Wrapper to mark a value as target endian, to guide conversion code

template<typename T> class base_endian {

 protected:
  T value;

  base_endian(T n) : value(n) {}

 public:
  // Setting to and testing against zero never needs swapping
  base_endian() : value(0) {}
  bool operator!() { return !value; }

  // Bitwise logic operations can be performed without swapping
  base_endian& operator|=(const base_endian& rhs) { value |= rhs.value; return *this; }
  base_endian& operator&=(const base_endian& rhs) { value &= rhs.value; return *this; }
  base_endian& operator^=(const base_endian& rhs) { value ^= rhs.value; return *this; }

  inline T from_be() { return ::from_be(value); }
  inline T from_le() { return ::from_le(value); }
};

template<typename T> class target_endian : public base_endian<T> {
 protected:
  target_endian(T n) : base_endian<T>(n) {}

 public:
  target_endian() {}

  static inline target_endian to_be(T n) { return target_endian(::to_be(n)); }
  static inline target_endian to_le(T n) { return target_endian(::to_le(n)); }

  // Useful values over which swapping is identity
  static const target_endian zero;
  static const target_endian all_ones;
};

template<typename T> const target_endian<T> target_endian<T>::zero = target_endian(T(0));
template<typename T> const target_endian<T> target_endian<T>::all_ones = target_endian(~T(0));


// Specializations with implicit conversions (no swap information needed)

template<> class target_endian<uint8_t> : public base_endian<uint8_t> {
 public:
  target_endian() {}
  target_endian(uint8_t n) : base_endian<uint8_t>(n) {}
  operator uint8_t() { return value; }

  static inline target_endian to_be(uint8_t n) { return target_endian(n); }
  static inline target_endian to_le(uint8_t n) { return target_endian(n); }
};

template<> class target_endian<int8_t> : public base_endian<int8_t> {
 public:
  target_endian() {}
  target_endian(int8_t n) : base_endian<int8_t>(n) {}
  operator int8_t() { return value; }

  static inline target_endian to_be(int8_t n) { return target_endian(n); }
  static inline target_endian to_le(int8_t n) { return target_endian(n); }
};

#endif
