#ifndef _RISCV_ABSTRACT_DEVICE_H
#define _RISCV_ABSTRACT_DEVICE_H

#include "decode.h"
#include <cstdint>
#include <cstddef>

class abstract_device_t {
 public:
  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) = 0;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) = 0;
  virtual ~abstract_device_t() {}
};

#endif
