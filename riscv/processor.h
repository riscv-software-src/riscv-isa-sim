#ifndef _RISCV_PROCESSOR_H
#define _RISCV_PROCESSOR_H

#include "decode.h"
#include <cstring>
#include "trap.h"
#include "config.h"

#define MAX_UTS 2048

class processor_t;
class mmu_t;
typedef reg_t (*insn_func_t)(processor_t*, insn_t, reg_t);
class sim_t;

// this class represents one processor in a RISC-V machine.
class processor_t
{
public:
  processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id);
  ~processor_t();

  void step(size_t n, bool noisy); // run for n cycles
  void deliver_ipi(); // register an interprocessor interrupt

private:
  sim_t& sim;
  mmu_t& mmu; // main memory is always accessed via the mmu

  // user-visible architected state
  reg_t XPR[NXPR];
  freg_t FPR[NFPR];
  reg_t pc;

  // counters
  reg_t cycle;

  // privileged control registers
  reg_t epc;
  reg_t badvaddr;
  reg_t evec;
  reg_t pcr_k0;
  reg_t pcr_k1;
  uint32_t cause;
  uint32_t interrupts_pending;
  uint32_t id;
  uint32_t sr; // only modify the status register using set_sr()
  uint32_t fsr;
  uint32_t count;
  uint32_t compare;

  // # of bits in an XPR (32 or 64). (redundant with sr)
  int xprlen;

  // is this processor running? (deliver_ipi() sets this)
  bool run;

  // functions
  void reset(); // resets architected state; halts processor if it was running
  void take_interrupt(); // take a trap if any interrupts are pending
  void set_sr(uint32_t val); // set the status register
  void set_fsr(uint32_t val); // set the floating-point status register
  void take_trap(trap_t t, bool noisy); // take an exception
  void disasm(insn_t insn, reg_t pc); // disassemble and print an instruction

  // vector stuff
  void vcfg();
  void setvl(int vlapp);

  reg_t vecbanks;
  uint32_t vecbanks_count;

  bool utmode;
  uint32_t utidx;
  int vlmax;
  int vl;
  int nxfpr_bank;
  int nxpr_use;
  int nfpr_use;
  processor_t* uts[MAX_UTS];

  // this constructor is used for each of the uts
  processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id, uint32_t _utidx);

  friend class sim_t;
  friend class mmu_t;

  #include "dispatch.h"
};

#ifndef RISCV_ENABLE_RVC
# define set_pc(x) \
  do { if((x) & (sizeof(insn_t)-1)) \
       { badvaddr = (x); throw trap_instruction_address_misaligned; } \
       npc = (x); \
     } while(0)
#else
# define set_pc(x) \
  do { if((x) & ((sr & SR_EC) ? 1 : 3)) \
       { badvaddr = (x); throw trap_instruction_address_misaligned; } \
       npc = (x); \
     } while(0)
#endif

#endif
