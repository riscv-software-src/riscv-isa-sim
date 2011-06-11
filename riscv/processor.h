#ifndef _RISCV_PROCESSOR_H
#define _RISCV_PROCESSOR_H

#include "decode.h"
#include <cstring>
#include "trap.h"
#include "mmu.h"
#include "icsim.h"

#define MAX_UTS 2048

#define DISPATCH_TABLE_SIZE 1024
class processor_t;
typedef reg_t (*insn_func_t)(processor_t*, insn_t, reg_t);

class sim_t;

class processor_t
{
public:
  processor_t(sim_t* _sim, char* _mem, size_t _memsz);
  ~processor_t();
  void init(uint32_t _id, icsim_t* defualt_icache, icsim_t* default_dcache);
  void step(size_t n, bool noisy);
  void deliver_ipi();

private:
  sim_t* sim;

  // architected state
  reg_t XPR[NXPR];
  freg_t FPR[NFPR];

  // privileged control registers
  reg_t pc;
  reg_t epc;
  reg_t badvaddr;
  reg_t cause;
  reg_t evec;
  reg_t tohost;
  reg_t fromhost;
  reg_t pcr_k0;
  reg_t pcr_k1;
  uint32_t id;
  uint32_t sr;
  uint32_t count;
  uint32_t compare;

  bool run;

  // unprivileged control registers
  uint32_t fsr;

  // # of bits in an XPR (32 or 64). (redundant with sr)
  int xprlen;

  // shared memory
  mmu_t mmu;

  // counters
  reg_t cycle;

  // functions
  void reset();
  void take_interrupt();
  void set_sr(uint32_t val);
  void set_fsr(uint32_t val);
  void take_trap(trap_t t, bool noisy);
  void disasm(insn_t insn, reg_t pc);

  // vector stuff
  void vcfg();
  void setvl(int vlapp);

  reg_t vecbanks;
  uint32_t vecbanks_count;

  bool utmode;
  int utidx;
  int vlmax;
  int vl;
  int nxfpr_bank;
  int nxpr_use;
  int nfpr_use;
  processor_t* uts[MAX_UTS];

  // cache sim
  icsim_t* icsim;
  icsim_t* dcsim;
  icsim_t* itlbsim;
  icsim_t* dtlbsim;

  friend class sim_t;

  static insn_func_t dispatch_table[DISPATCH_TABLE_SIZE];
  reg_t dispatch(insn_t insn, reg_t pc);
  static void initialize_dispatch_table();

  #define DECLARE_INSN(name, m, o) reg_t insn_func_ ## name (insn_t, reg_t);
  #include "opcodes.h"
  #undef DECLARE_INSN
};

#endif
