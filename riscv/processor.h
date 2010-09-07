#ifndef _RISCV_PROCESSOR_H
#define _RISCV_PROCESSOR_H

#include <cstring>
#include "decode.h"
#include "trap.h"
#include "mmu.h"

class sim_t;

class processor_t
{
public:
  processor_t(sim_t* _sim, char* _mem, size_t _memsz);
  void init(uint32_t _id);
  void step(size_t n, bool noisy);

private:
  sim_t* sim;

  // architected state
  reg_t R[NGPR];
  freg_t FR[NFPR];

  // privileged control registers
  reg_t pc;
  reg_t epc;
  reg_t badvaddr;
  reg_t ebase;
  reg_t pcr_k0;
  reg_t pcr_k1;
  uint32_t id;
  uint32_t sr;

  // unprivileged control registers
  uint32_t tid;
  uint32_t fsr;

  // 32-bit or 64-bit mode (redundant with sr)
  int gprlen;

  // shared memory
  mmu_t mmu;

  // counters
  reg_t counters[32];

  // functions
  void set_sr(uint32_t val);
  void set_fsr(uint32_t val);
  void take_trap(trap_t t, bool noisy);
  void disasm(insn_t insn, reg_t pc);

  friend class sim_t;
};

#endif
