// See LICENSE for license details.

#ifndef _RISCV_BYTEORDER_H
#define _RISCV_BYTEORDER_H

#include "config.h"
#include <stdint.h>

static inline uint8_t swap(uint8_t n) { return n; }
static inline uint16_t swap(uint16_t n) { return __builtin_bswap16(n); }
static inline uint32_t swap(uint32_t n) { return __builtin_bswap32(n); }
static inline uint64_t swap(uint64_t n) { return __builtin_bswap64(n); }
static inline int8_t swap(int8_t n) { return n; }
static inline int16_t swap(int16_t n) { return __builtin_bswap16(n); }
static inline int32_t swap(int32_t n) { return __builtin_bswap32(n); }
static inline int64_t swap(int64_t n) { return __builtin_bswap64(n); }

#ifdef WORDS_BIGENDIAN
template<typename T> static inline T from_be(T n) { return n; }
template<typename T> static inline T to_be(T n) { return n; }
template<typename T> static inline T from_le(T n) { return swap(n); }
template<typename T> static inline T to_le(T n) { return swap(n); }
#else
template<typename T> static inline T from_le(T n) { return n; }
template<typename T> static inline T to_le(T n) { return n; }
template<typename T> static inline T from_be(T n) { return swap(n); }
template<typename T> static inline T to_be(T n) { return swap(n); }
#endif

#endif
