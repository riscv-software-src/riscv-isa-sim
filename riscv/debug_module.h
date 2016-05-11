// See LICENSE for license details.
#ifndef _RISCV_DEBUG_MODULE_H
#define _RISCV_DEBUG_MODULE_H

#include <set>

#include "devices.h"

class debug_module_t : public abstract_device_t
{
  public:
    bool load(reg_t addr, size_t len, uint8_t* bytes);
    bool store(reg_t addr, size_t len, const uint8_t* bytes);

    void ram_write32(unsigned int index, uint32_t value);
    uint32_t ram_read32(unsigned int index);

    void set_interrupt(uint32_t hartid) {
      interrupt.insert(hartid);
    }
    void clear_interrupt(uint32_t hartid) {
      interrupt.erase(hartid);
    }
    bool get_interrupt(uint32_t hartid) const {
      return interrupt.find(hartid) != interrupt.end();
    }

    void set_halt_notification(uint32_t hartid) {
      halt_notification.insert(hartid);
    }
    void clear_halt_notification(uint32_t hartid) {
      halt_notification.erase(hartid);
    }
    bool get_halt_notification(uint32_t hartid) const {
      return halt_notification.find(hartid) != halt_notification.end();
    }

  private:
    // Track which interrupts from module to debugger are set.
    std::set<uint32_t> interrupt;
    // Track which halt notifications from debugger to module are set.
    std::set<uint32_t> halt_notification;
    char debug_ram[DEBUG_RAM_SIZE];
};

#endif
