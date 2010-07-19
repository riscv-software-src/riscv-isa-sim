#ifndef _RISCV_PROCESSOR_H
#define _RISCV_PROCESSOR_H

#include <cstring>
#include "decode.h"
#include "trap.h"
#include "mmu.h"

class processor_t
{
public:
  processor_t(int _id, char* _mem, size_t _memsz);
  void step(size_t n, bool noisy);

private:
  // architected state
  reg_t R[NGPR];
  reg_t pc;
  reg_t epc;
  reg_t badvaddr;
  reg_t ebase;
  uint32_t id;
  uint32_t sr;
  int gprlen;

  // shared memory
  mmu_t mmu;

  // counters
  reg_t counters[32];

  // functions
  void set_sr(uint32_t val);
  void take_trap(trap_t t);
  void disasm(insn_t insn, reg_t pc);

  friend class sim_t;
};

#endif
