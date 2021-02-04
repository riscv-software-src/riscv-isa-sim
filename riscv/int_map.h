#ifndef _RISCV_INT_MAP_H
#define _RISCV_INT_MAP_H

#include <cstdint>
#include <climits>
#include <cassert>

// Like std::map, but keys are integers
template<typename K, typename V, int lg_radix = 11>
class int_map {
 public:
  int_map(int lg_n = sizeof(K) * CHAR_BIT)
    : lg_n(lg_n)
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

    if (array[idx] == nullptr)
      array[idx] = new int_map<K, V, lg_radix>(lg_n - lg_radix);

    return array[idx]->lookup(k >> lg_radix);
  }

 private:
  int lg_n;
  int_map<K, V, lg_radix>* array[size_t(1) << lg_radix];
};

#endif
