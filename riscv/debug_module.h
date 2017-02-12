// See LICENSE for license details.
#ifndef _RISCV_DEBUG_MODULE_H
#define _RISCV_DEBUG_MODULE_H

#include <set>

#include "devices.h"

class sim_t;

typedef struct {
  bool haltreq;
  bool reset;
  bool dmactive;
  enum {
    HARTSTATUS_HALTED,
    HARTSTATUS_RUNNING,
    HARTSTATUS_UNAVAILABLE,
    HARTSTATUS_NOTEXIST
  } hartstatus;
  unsigned hartsel;
  bool authenticated;
  bool authbusy;
  enum {
    AUTHTYPE_NOAUTH,
    AUTHTYPE_PASSWORD,
    AUTHTYPE_CHALLENGE
  } authtype;
  unsigned version;
} dmcontrol_t;

class debug_module_t : public abstract_device_t
{
  public:
    debug_module_t(sim_t *sim);

    bool load(reg_t addr, size_t len, uint8_t* bytes);
    bool store(reg_t addr, size_t len, const uint8_t* bytes);

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

    // Debug Module Interface that the debugger (in our case through JTAG DTM)
    // uses to access the DM.
    // Return true for success, false for failure.
    bool dmi_read(unsigned address, uint32_t *value);
    bool dmi_write(unsigned address, uint32_t value);

  private:
    sim_t *sim;
    // Track which interrupts from module to debugger are set.
    std::set<uint32_t> interrupt;
    // Track which halt notifications from debugger to module are set.
    std::set<uint32_t> halt_notification;
    uint8_t debug_rom_entry[1024 * 4];
    bool halted[1024];

    void write32(uint8_t *rom, unsigned int index, uint32_t value);
    uint32_t read32(uint8_t *rom, unsigned int index);

    static const unsigned datacount = 8;
    static const unsigned progsize = 8;

    dmcontrol_t dmcontrol;
    uint32_t abstractcs;
    uint32_t data[datacount];
    uint32_t ibuf[progsize];

    processor_t *current_proc() const;
    void reset();
    bool perform_abstract_command(uint32_t command);
};

#endif
