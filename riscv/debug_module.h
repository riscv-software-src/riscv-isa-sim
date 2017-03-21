// See LICENSE for license details.
#ifndef _RISCV_DEBUG_MODULE_H
#define _RISCV_DEBUG_MODULE_H

#include <set>

#include "devices.h"

class sim_t;

typedef struct {
  bool haltreq;
  bool resumereq;
  unsigned hartsel;
  bool hartreset;
  bool dmactive;
  bool ndmreset;
} dmcontrol_t;

typedef struct {
  bool allnonexistant;
  bool anynonexistant;
  bool allunavail;
  bool anyunavail;
  bool allrunning;
  bool anyrunning;
  bool allhalted;
  bool anyhalted;
  bool authenticated;
  bool authbusy;
  bool cfgstrvalid;
  unsigned versionhi;
  unsigned versionlo;
} dmstatus_t;

typedef struct {
  bool busy;
  unsigned datacount;
  unsigned progsize;
  enum {
    CMDERR_NONE = 0,
    CMDERR_BUSY = 1,
    CMDERR_NOTSUP = 2,
    CMDERR_EXCEPTION = 3,
    CMDERR_HALTRESUME = 4,
    CMDERR_OTHER = 7
  } cmderr;
} abstractcs_t;

typedef struct {
  unsigned autoexecprogbuf;
  unsigned autoexecdata;
} abstractauto_t;

class debug_module_data_t : public abstract_device_t
{
  public:
    debug_module_data_t();

    bool load(reg_t addr, size_t len, uint8_t* bytes);
    bool store(reg_t addr, size_t len, const uint8_t* bytes);

    uint32_t read32(reg_t addr) const;
    void write32(reg_t addr, uint32_t value);

    uint8_t data[DEBUG_EXCHANGE_SIZE];
};

class debug_module_t : public abstract_device_t
{
  public:
    debug_module_t(sim_t *sim);

    void add_device(bus_t *bus);

    bool load(reg_t addr, size_t len, uint8_t* bytes);
    bool store(reg_t addr, size_t len, const uint8_t* bytes);

    // Debug Module Interface that the debugger (in our case through JTAG DTM)
    // uses to access the DM.
    // Return true for success, false for failure.
    bool dmi_read(unsigned address, uint32_t *value);
    bool dmi_write(unsigned address, uint32_t value);

  private:
    static const unsigned progsize = 8;

    sim_t *sim;

    uint8_t debug_rom_entry[DEBUG_ROM_ENTRY_SIZE];
    uint8_t debug_rom_code[DEBUG_ROM_CODE_SIZE];
    uint8_t debug_rom_exception[DEBUG_ROM_EXCEPTION_SIZE];
    uint8_t program_buffer[progsize * 4];
    bool halted[1024];
    debug_module_data_t dmdata;
    // Instruction that will be placed at the current hart's ROM entry address
    // after the current action has completed.
    uint32_t next_action;
    bool action_executed;

    void write32(uint8_t *rom, unsigned int index, uint32_t value);
    uint32_t read32(uint8_t *rom, unsigned int index);

    dmcontrol_t dmcontrol;
    dmstatus_t dmstatus;
    abstractcs_t abstractcs;
    abstractauto_t abstractauto;
    uint32_t command;

    processor_t *current_proc() const;
    void reset();
    bool perform_abstract_command();
};

#endif
