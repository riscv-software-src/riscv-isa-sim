#ifndef __DIFFTRACE_H
#define __DIFFTRACE_H

#include <bits/stdc++.h>
#include <queue>
#include "difftest-def.h"

class store_trace_t {
public:
  uint64_t paddr;
  uint64_t data;
  uint8_t  mask;

  store_trace_t(uint64_t paddr, uint64_t data, uint8_t mask) : paddr(paddr), data(data), mask(mask) {
    do_align();
  }
  store_trace_t(uint64_t paddr, uint64_t data, int len) : paddr(paddr), data(data), mask((1U << len) - 1) {
    if (len != 8) {
      this->data &= (1UL << (len * 8)) - 1UL;
    }
    do_align();
  };

private:
  void do_align() {
    uint64_t offset = paddr % 8UL;
    if (offset) {
      int len = std::log2((long)mask + 1);
      paddr = paddr - offset;
      data &= (1UL << (len * 8)) - 1UL;
      data <<= offset << 3;
      mask <<= offset;
    }
  }
};

class diff_trace_t
{
private:
  std::queue<store_trace_t> store_trace;

  enum class MemAccessType { INSTRUCTION, LOAD, STORE };
  static const char *accessTypeString(MemAccessType value) {
    switch (value) {
      case MemAccessType::INSTRUCTION: return "instr";
      case MemAccessType::LOAD:        return "load";
      case MemAccessType::STORE:       return "store";
      default:                         return "unknown";
    }
  }

  void difftest_log_mem(MemAccessType t, uint64_t paddr, uint64_t data, int len) {
    difftest_log("mem_%-5s addr: 0x%lx, data: 0x%016lx, len: %d", accessTypeString(t), paddr, data, len);
    if (t == MemAccessType::STORE) {
      bool do_trace = !is_amo;
#ifdef CONFIG_DIFF_AMO_STORE
      do_trace = true;
#endif
      if (do_trace) {
        store_trace_t trace{paddr, data, len};
        store_trace.push(trace);
      }
      is_amo = false;
    }
  }

  void difftest_log_mem(MemAccessType t, uint64_t paddr, void *data, int len) {
    difftest_log_mem(t, paddr, *(const uint64_t *)data, len);
  }

public:
  bool enable_difftest_logs = false;
  bool is_amo = false;

  void difftest_log(const char *__restrict __fmt, ...) {
    if (unlikely(enable_difftest_logs)) {
      va_list args;
      va_start(args, __fmt);
      fprintf(stderr, "[Spike] ");
      vfprintf(stderr, __fmt, args);
      fprintf(stderr, "\n");
      fflush(stderr);
      va_end(args);
    }
  }

#define __DIFFTEST_LOG_INTERFACE(name, type)                                      \
  void inline difftest_log_mem_##name(uint64_t paddr, void *data, int len) {      \
    difftest_log_mem(MemAccessType::type, paddr, *(const uint64_t *)data, len); \
  }

  __DIFFTEST_LOG_INTERFACE(instr, INSTRUCTION)
  __DIFFTEST_LOG_INTERFACE(load, LOAD)
  __DIFFTEST_LOG_INTERFACE(store, STORE)

  int dut_store_commit(uint64_t *addr, uint64_t *data, uint8_t *mask) {
    if (store_trace.empty()) {
      printf("Store commit error: the store trace is empty.\n");
      return -1;
    }

    store_trace_t ref = store_trace.front();
    store_trace_t dut{*addr, *data, *mask};
    if (ref.paddr != dut.paddr || ref.data != dut.data || ref.mask != dut.mask) {
      *addr = ref.paddr;
      *data = ref.data;
      *mask = ref.mask;
      return -1;
    }

    store_trace.pop();
    return 0;
  }
};

#endif
