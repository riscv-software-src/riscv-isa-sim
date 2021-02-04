#ifndef _RISCV_INT_MAP_H
#define _RISCV_INT_MAP_H

#include "common.h"
#include <cstdint>
#include <climits>

// Like std::map, but keys are integers
template<typename K, typename V, int lg_n = sizeof(K) * CHAR_BIT, int lg_radix = 11>
class int_map {
 public:
  int_map()
  {
    memset(array, 0, sizeof(array));
  }

  V* lookup(K k)
  {
    K idx = k & ((K(1) << lg_radix) - 1);

    if (lg_n <= lg_radix) {
      static_assert(sizeof(void*) == sizeof(V), "int_map: V must be pointer-sized");
      return reinterpret_cast<V*>(&array[idx]);
    }

    if (unlikely(array[idx] == nullptr))
      array[idx] = new int_map<K, V, lg_n < 0 ? 0 : lg_n - lg_radix, lg_radix>;

    return array[idx]->lookup(k >> lg_radix);
  }

 private:
  int_map<K, V, lg_n < 0 ? 0 : lg_n - lg_radix, lg_radix>* array[size_t(1) << lg_radix];
};

#endif
