#ifndef _RISCV_ABSTRACT_INTERRUPT_CONTROLLER_H
#define _RISCV_ABSTRACT_INTERRUPT_CONTROLLER_H

#include "decode.h"
#include <cstdint>
#include <cstddef>

class abstract_interrupt_controller_t {
 public:
  virtual void set_interrupt_level(uint32_t interrupt_id, int level) = 0;
  virtual ~abstract_interrupt_controller_t() {}
};

#endif
