// See LICENSE for license details.

#ifndef _RISCV_BLOOM_FILTER_H
#define _RISCV_BLOOM_FILTER_H

#include <bitset>
#include <cstdint>

struct simple_hash1 {
  uint64_t operator()(uint64_t x) const
  {
    x = (x ^ (x >> 33)) * 0xff51afd7ed558ccd;
    x = (x ^ (x >> 33)) * 0xc4ceb9fe1a85ec53;
    return x ^ (x >> 33);
  }
};

struct simple_hash2 {
  uint64_t operator()(uint64_t x) const
  {
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049b13c66a8ed;
    return x ^ (x >> 31);
  }
};

template <typename T, typename H1, typename H2, size_t M, size_t K> // M: bit array size, K: number of hash functions
class bloom_filter_t {
 public:
  void clear()
  {
    bits.reset();
  }

  void insert(T value)
  {
    uint64_t h1 = H1()(value);
    uint64_t h2 = H2()(value);

    for (size_t i = 0; i < K; i++) {
      size_t idx = (h1 + i * h2) % M;
      bits[idx] = true;
    }
  }

  bool contains(T value) const
  {
    uint64_t h1 = H1()(value);
    uint64_t h2 = H2()(value);

    for (size_t i = 0; i < K; i++) {
      size_t idx = (h1 + i * h2) % M;
      if (!bits[idx])
        return false;
    }

    return true;
  }

 private:
  std::bitset<M> bits;
};

#endif
