#ifndef __DUMMY_DEBUG_H
#define __DUMMY_DEBUG_H

#include "sim.h"
#include "abstract_device.h"
#include "mmu.h"

#define DM_BASE_ADDR 0x38020000L
#define DM_END_ADDR 0x38021000L
#define DUMMY_MEM_SIZE_BYTE (DM_END_ADDR - DM_BASE_ADDR) / sizeof(uint8_t)

class dummy_debug_t : public abstract_device_t
{
  public:
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  ~dummy_debug_t();
  // we create a mem Region that is updated on every dut if/load/store
  uint8_t dummy_debug_mem[DUMMY_MEM_SIZE_BYTE];
  bool update_dummy_mem(reg_t addr, size_t len, const uint8_t* bytes);
};

#endif
