#ifndef __DIFFTEST_H
#define __DIFFTEST_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include "dummy_debug.h"
#include "sim.h"

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };
#define FMT_WORD "0x%016lx"

# define DIFFTEST_REG_SIZE (sizeof(uint64_t) * 33) // GRPs + pc

#ifndef DIFFTEST_LOG_FILE
#define DIFFTEST_LOG_FILE nullptr
#endif

#if defined(CPU_NUTSHELL)
#elif defined(CPU_XIANGSHAN)
#elif defined(CPU_ROCKET_CHIP)
#else
// This is the default CPU
#define CPU_NUTSHELL
#endif

#if defined(CPU_XIANGSHAN) || defined(CPU_ROCKET_CHIP)
#define CONFIG_DIFF_FPU
#endif

#if defined(CPU_XIANGSHAN)
#define CONFIG_DIFF_DEBUG_MODE
#endif


#if defined(CPU_NUTSHELL)
#define CONFIG_DIFF_ISA_STRING "rv64imaczicsr_zifencei"
#define CONFIG_MEMORY_SIZE     (2 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x40000000UL
#define CONFIG_FLASH_SIZE      0x1000UL
#elif defined(CPU_XIANGSHAN)
#define CONFIG_DIFF_ISA_STRING "RV64IMAFDC_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zknd_zkne_zknh_zksed_zksh_svinval"
#define CONFIG_MEMORY_SIZE     (16 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x10000000UL
#define CONFIG_FLASH_SIZE      0x100000UL
#elif defined(CPU_ROCKET_CHIP)
#define CONFIG_DIFF_ISA_STRING "rv64imafdczicsr_zifencei_zihpm"
#define CONFIG_MEMORY_SIZE     (2 * 1024 * 1024 * 1024UL)
#define CONFIG_FLASH_BASE      0x10000000UL
#define CONFIG_FLASH_SIZE      0x10000UL
#endif

typedef struct {
  uint64_t gpr[32];
#ifdef CONFIG_DIFF_FPU
  uint64_t fpr[32];
#endif
  uint64_t priv;
  uint64_t mstatus;
  uint64_t sstatus;
  uint64_t mepc;
  uint64_t sepc;
  uint64_t mtval;
  uint64_t stval;
  uint64_t mtvec;
  uint64_t stvec;
  uint64_t mcause;
  uint64_t scause;
  uint64_t satp;
  uint64_t mip;
  uint64_t mie;
  uint64_t mscratch;
  uint64_t sscratch;
  uint64_t mideleg;
  uint64_t medeleg;
  uint64_t pc;
#ifdef CONFIG_DIFF_DEBUG_MODE
  uint64_t debugMode;
  uint64_t dcsr;
  uint64_t dpc;
  uint64_t dscratch0;
  uint64_t dscratch1;
#endif // CONFIG_DIFF_DEBUG_MODE
} diff_context_t;

class DifftestRefConfig {
public:
  bool ignore_illegal_mem_access = false;
  bool debug_difftest = false;
};

class DifftestRef {
public:
  DifftestRef();
  ~DifftestRef();
  void step(uint64_t n);
  void get_regs(diff_context_t *ctx);
  void set_regs(diff_context_t *ctx);
  void memcpy_from_dut(reg_t dest, void* src, size_t n);
  void debug_memcpy_from_dut(reg_t dest, void* src, size_t n);
  int store_commit(uint64_t *addr, uint64_t *data, uint8_t *mask);
  void raise_intr(uint64_t no);
  void display();
  void update_dynamic_config(void* config) {
#ifdef RISCV_ENABLE_COMMITLOG
  p->enable_log_commits();
#endif
    auto c = (DifftestRefConfig *)config;
    sim->enable_difftest_logs = c->debug_difftest;
  }


private:
  const cfg_t *cfg;
  const std::vector<std::pair<reg_t, mem_t*>> mems;
  const std::vector<std::pair<reg_t, abstract_device_t*>> plugin_devices;
  sim_t * const sim;
  processor_t * const p;
  state_t * const state;

  const cfg_t *create_cfg();
  sim_t *create_sim(const cfg_t *cfg);
};

#endif
