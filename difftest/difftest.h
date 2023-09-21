#ifndef __DIFFTEST_H
#define __DIFFTEST_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include "difftest-def.h"
#include "dummy_debug.h"
#include "sim.h"

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };
#define FMT_WORD "0x%016lx"

# define DIFFTEST_REG_SIZE (sizeof(uint64_t) * 33) // GRPs + pc

#ifndef DIFFTEST_LOG_FILE
#define DIFFTEST_LOG_FILE nullptr
#endif

/***************DON'T CHANGE ORDER****************************/
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
#ifdef CONFIG_DIFF_RVV
  #define VLEN 128
  #define VENUM64 (VLEN/64)
  #define VENUM32 (VLEN/32)
  #define VENUM16 (VLEN/16)
  #define VENUM8  (VLEN/8)

  union {
    uint64_t _64[VENUM64];
    uint32_t _32[VENUM32];
    uint16_t _16[VENUM16];
    uint8_t  _8[VENUM8];
  } vr[32];

  uint64_t vstart;
  uint64_t vxsat;
  uint64_t vxrm;
  uint64_t vcsr;
  uint64_t vl;
  uint64_t vtype;
  uint64_t vlenb;
#endif // CONFIG_DIFF_RVV

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

class DifftestUarchStatus {
public:
  uint64_t sc_failed = 0;
};

class DifftestRef {
public:
  DifftestRef();
  ~DifftestRef();
  void step(uint64_t n);
  void skip_one(bool isRVC, bool wen, uint32_t wdest, uint64_t wdata);
  void get_regs(diff_context_t *ctx);
  void set_regs(diff_context_t *ctx, bool on_demand);
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
  void update_uarch_status(void *status) {
    auto s = (DifftestUarchStatus *)status;
    if (s->sc_failed) {
      sim->sc_failed = true;
    }
  }
  inline int disambiguation_state() {
    return sim->in_ambiguation_state();
  }

private:
  const cfg_t *cfg;
  const std::vector<std::pair<reg_t, mem_t*>> mems;
  const std::vector<std::pair<reg_t, abstract_device_t*>> plugin_devices;
  sim_t * const sim;
  processor_t * const p;
  state_t * const state;

  const cfg_t *create_cfg();
  const std::vector<std::pair<reg_t, abstract_device_t*>> create_devices();
  sim_t *create_sim(const cfg_t *cfg);
};

#endif
