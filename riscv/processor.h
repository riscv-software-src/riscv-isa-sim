// See LICENSE for license details.

#ifndef _RISCV_PROCESSOR_H
#define _RISCV_PROCESSOR_H

#include "decode.h"
#include <cstring>
#include "config.h"
#include <map>

class processor_t;
class mmu_t;
typedef reg_t (*insn_func_t)(processor_t*, insn_t, reg_t);
class sim_t;
class trap_t;
class extension_t;

struct insn_desc_t
{
  uint32_t match;
  uint32_t mask;
  insn_func_t rv32;
  insn_func_t rv64;
};

// architectural state of a RISC-V hart
struct state_t
{
  void reset();

  // user-visible state
  reg_t pc;
  regfile_t<reg_t, NXPR, true> XPR;
  regfile_t<freg_t, NFPR, false> FPR;
  reg_t cycle;

  // privileged control registers
  reg_t epc;
  reg_t badvaddr;
  reg_t evec;
  reg_t ptbr;
  reg_t pcr_k0;
  reg_t pcr_k1;
  reg_t cause;
  reg_t tohost;
  reg_t fromhost;
  uint32_t sr; // only modify the status register using set_pcr()
  uint32_t fsr;
  uint32_t count;
  uint32_t compare;

  reg_t load_reservation;
};

// this class represents one processor in a RISC-V machine.
class processor_t
{
public:
  processor_t(sim_t* _sim, mmu_t* _mmu, uint32_t _id);
  ~processor_t();

  void reset(bool value);
  void step(size_t n, bool noisy); // run for n cycles
  void deliver_ipi(); // register an interprocessor interrupt
  bool running() { return run; }
  reg_t set_pcr(int which, reg_t val);
  uint32_t set_fsr(uint32_t val); // set the floating-point status register
  void set_interrupt(int which, bool on);
  reg_t get_pcr(int which);
  uint32_t get_fsr() { return state.fsr; }
  mmu_t* get_mmu() { return mmu; }
  state_t* get_state() { return &state; }
  extension_t* get_extension() { return ext; }
  void yield_load_reservation() { state.load_reservation = (reg_t)-1; }

  void register_insn(insn_desc_t);
  void register_extension(extension_t*);

private:
  sim_t* sim;
  mmu_t* mmu; // main memory is always accessed via the mmu
  extension_t* ext;
  state_t state;
  uint32_t id;
  bool run; // !reset

  unsigned opcode_bits;
  std::multimap<uint32_t, insn_desc_t> opcode_map;

  void take_interrupt(); // take a trap if any interrupts are pending
  void take_trap(reg_t pc, trap_t& t, bool noisy); // take an exception
  void disasm(insn_t insn, reg_t pc); // disassemble and print an instruction

  friend class sim_t;
  friend class mmu_t;
  friend class extension_t;
  friend class htif_isasim_t;

  insn_func_t decode_insn(insn_t insn);
};

reg_t illegal_instruction(processor_t* p, insn_t insn, reg_t pc);

#define REGISTER_INSN(proc, name, match, mask) \
  extern reg_t rv32_##name(processor_t*, insn_t, reg_t); \
  extern reg_t rv64_##name(processor_t*, insn_t, reg_t); \
  proc->register_insn((insn_desc_t){match, mask, rv32_##name, rv64_##name});

#endif
