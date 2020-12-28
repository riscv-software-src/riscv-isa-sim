// See LICENSE for license details.

#include "byteorder.h"
#include "config.h"

#ifdef WORDS_BIGENDIAN
#define ENDIANESS_CONV(type)                 \
    template <typename type>                 \
    type from_be(type n) { return n; }       \
    template <typename type>                 \
    type to_be(type n) { return n; }         \
    template <typename type>                 \
    type from_le(type n) { return swap(n); } \
    template <typename type>                 \
    type to_le(type n) { return swap(n); }
#else
#define ENDIANESS_CONV(type)                 \
    template <typename type>                 \
    type from_be(type n) { return swap(n); } \
    template <typename type>                 \
    type to_be(type n) { return swap(n); }   \
    template <typename type>                 \
    type from_le(type n) { return n; }       \
    template <typename type>                 \
    type to_le(type n) { return n; }
#endif

ENDIANESS_CONV(uint8_t)
ENDIANESS_CONV(uint16_t)
ENDIANESS_CONV(uint32_t)
ENDIANESS_CONV(uint64_t)
ENDIANESS_CONV(int8_t)
ENDIANESS_CONV(int16_t)
ENDIANESS_CONV(int32_t)
ENDIANESS_CONV(int64_t)
