// See LICENSE for license details.

#ifndef _RISCV_SIMIF_H
#define _RISCV_SIMIF_H

#include <map>
#include "decode.h"
#include "cfg.h"

class processor_t;

// this is the interface to the simulator used by the processors and memory
class simif_t
{
public:
  // should return NULL for MMIO addresses
  virtual char* addr_to_mem(reg_t paddr) = 0;
  virtual bool reservable(reg_t paddr) { return addr_to_mem(paddr); }
  // used for MMIO addresses
  virtual bool mmio_fetch(reg_t paddr, size_t len, uint8_t* bytes) { return mmio_load(paddr, len, bytes); }
  virtual bool mmio_load(reg_t paddr, size_t len, uint8_t* bytes) = 0;
  virtual bool mmio_store(reg_t paddr, size_t len, const uint8_t* bytes) = 0;
  // Callback for processors to let the simulation know they were reset.
  virtual void proc_reset(unsigned id) = 0;

  virtual processor_t* get_core(size_t i) = 0;
  virtual const cfg_t &get_cfg() = 0;
  virtual unsigned nprocs() const = 0;
  virtual const std::map<size_t, processor_t*>& get_harts() = 0;

  virtual const char* get_symbol(uint64_t paddr) = 0;

  virtual ~simif_t() = default;

};

#endif
