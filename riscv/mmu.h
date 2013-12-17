// See LICENSE for license details.

#ifndef _RISCV_MMU_H
#define _RISCV_MMU_H

#include "decode.h"
#include "trap.h"
#include "common.h"
#include "config.h"
#include "processor.h"
#include "memtracer.h"
#include <vector>

// virtual memory configuration
typedef reg_t pte_t;
const reg_t LEVELS = sizeof(pte_t) == 8 ? 3 : 2;
const reg_t PTIDXBITS = 10;
const reg_t PGSHIFT = PTIDXBITS + (sizeof(pte_t) == 8 ? 3 : 2);
const reg_t PGSIZE = 1 << PGSHIFT;
const reg_t VPN_BITS = PTIDXBITS * LEVELS;
const reg_t PPN_BITS = 8*sizeof(reg_t) - PGSHIFT;
const reg_t VA_BITS = VPN_BITS + PGSHIFT;

// this class implements a processor's port into the virtual memory system.
// an MMU and instruction cache are maintained for simulator performance.
class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz);
  ~mmu_t();

  // template for functions that load an aligned value from memory
  #define load_func(type) \
    type##_t load_##type(reg_t addr) __attribute__((always_inline)) { \
      if(unlikely(addr % sizeof(type##_t))) \
        throw trap_load_address_misaligned(addr); \
      void* paddr = translate(addr, sizeof(type##_t), false, false); \
      return *(type##_t*)paddr; \
    }

  // load value from memory at aligned address; zero extend to register width
  load_func(uint8)
  load_func(uint16)
  load_func(uint32)
  load_func(uint64)

  // load value from memory at aligned address; sign extend to register width
  load_func(int8)
  load_func(int16)
  load_func(int32)
  load_func(int64)

  // template for functions that store an aligned value to memory
  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      if(unlikely(addr % sizeof(type##_t))) \
        throw trap_store_address_misaligned(addr); \
      void* paddr = translate(addr, sizeof(type##_t), true, false); \
      *(type##_t*)paddr = val; \
    }

  // store value to memory at aligned address
  store_func(uint8)
  store_func(uint16)
  store_func(uint32)
  store_func(uint64)

  struct insn_fetch_t
  {
    insn_func_t func;
    union {
      insn_t insn;
      uint_fast32_t pad;
    } insn;
  };

  // load instruction from memory at aligned address.
  inline insn_fetch_t load_insn(reg_t addr)
  {
    reg_t offset = addr & (sizeof(insn_t) * (ICACHE_ENTRIES-1));
    offset *= sizeof(icache_entry_t) / sizeof(insn_t);
    icache_entry_t* entry = (icache_entry_t*)((char*)icache + offset);
    insn_fetch_t data = entry->data;
    if (likely(entry->tag == addr))
      return data;

    void* iaddr = translate(addr, sizeof(insn_t), false, true);
    insn_fetch_t fetch;
    fetch.insn.pad = *(decltype(fetch.insn.insn.bits())*)iaddr;
    fetch.func = proc->decode_insn(fetch.insn.insn);

    entry->tag = addr;
    entry->data = fetch;

    reg_t paddr = (char*)iaddr - mem;
    if (!tracer.empty() && tracer.interested_in_range(paddr, paddr + sizeof(insn_t), false, true))
    {
      entry->tag = -1;
      tracer.trace(paddr, sizeof(insn_t), false, true);
    }
    return entry->data;
  }

  void set_processor(processor_t* p) { proc = p; flush_tlb(); }

  void flush_tlb();
  void flush_icache();

  void register_memtracer(memtracer_t*);

private:
  char* mem;
  size_t memsz;
  processor_t* proc;
  memtracer_list_t tracer;

  // implement an instruction cache for simulator performance
  static const reg_t ICACHE_ENTRIES = 2048;
  struct icache_entry_t {
    reg_t tag;
    reg_t pad;
    insn_fetch_t data;
  };
  icache_entry_t icache[ICACHE_ENTRIES];

  // implement a TLB for simulator performance
  static const reg_t TLB_ENTRIES = 256;
  char* tlb_data[TLB_ENTRIES];
  reg_t tlb_insn_tag[TLB_ENTRIES];
  reg_t tlb_load_tag[TLB_ENTRIES];
  reg_t tlb_store_tag[TLB_ENTRIES];

  // finish translation on a TLB miss and upate the TLB
  void* refill_tlb(reg_t addr, reg_t bytes, bool store, bool fetch);

  // perform a page table walk for a given virtual address
  pte_t walk(reg_t addr);

  // translate a virtual address to a physical address
  void* translate(reg_t addr, reg_t bytes, bool store, bool fetch)
    __attribute__((always_inline))
  {
    reg_t idx = (addr >> PGSHIFT) % TLB_ENTRIES;
    reg_t expected_tag = addr & ~(PGSIZE-1);

    reg_t* tlb_tag = fetch ? tlb_insn_tag : store ? tlb_store_tag :tlb_load_tag;
    void* data = tlb_data[idx] + addr;
    if (likely(tlb_tag[idx] == expected_tag))
      return data;

    return refill_tlb(addr, bytes, store, fetch);
  }
  
  friend class processor_t;
};

#endif
