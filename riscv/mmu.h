#ifndef _RISCV_MMU_H
#define _RISCV_MMU_H

#include "decode.h"
#include "trap.h"
#include "common.h"
#include "config.h"
#include "processor.h"
#include "memtracer.h"
#include <vector>

class processor_t;

// virtual memory configuration
typedef reg_t pte_t;
const reg_t LEVELS = sizeof(pte_t) == sizeof(uint64_t) ? 3 : 2;
const reg_t PGSHIFT = 13;
const reg_t PGSIZE = 1 << PGSHIFT;
const reg_t PTIDXBITS = PGSHIFT - (sizeof(pte_t) == 8 ? 3 : 2);
const reg_t VPN_BITS = PTIDXBITS * LEVELS;
const reg_t PPN_BITS = 8*sizeof(reg_t) - PGSHIFT;
const reg_t VA_BITS = VPN_BITS + PGSHIFT;

// page table entry (PTE) fields
#define PTE_T    0x001 // Entry is a page Table descriptor
#define PTE_E    0x002 // Entry is a page table Entry
#define PTE_R    0x004 // Referenced
#define PTE_D    0x008 // Dirty
#define PTE_UX   0x010 // User eXecute permission
#define PTE_UW   0x020 // User Read permission
#define PTE_UR   0x040 // User Write permission
#define PTE_SX   0x080 // Supervisor eXecute permission
#define PTE_SW   0x100 // Supervisor Read permission
#define PTE_SR   0x200 // Supervisor Write permission
#define PTE_PERM (PTE_SR | PTE_SW | PTE_SX | PTE_UR | PTE_UW | PTE_UX)
#define PTE_PPN_SHIFT  13 // LSB of physical page number in the PTE

// this class implements a processor's port into the virtual memory system.
// an MMU and instruction cache are maintained for simulator performance.
class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz);
  ~mmu_t();

  // template for functions that load an aligned value from memory
  #define load_func(type) \
    type##_t load_##type(reg_t addr) { \
      if(unlikely(addr % sizeof(type##_t))) \
      { \
        badvaddr = addr; \
        throw trap_load_address_misaligned; \
      } \
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
      { \
        badvaddr = addr; \
        throw trap_store_address_misaligned; \
      } \
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
    insn_t insn;
    insn_func_t func;
  };

  // load instruction from memory at aligned address.
  // (needed because instruction alignment requirement is variable
  // if RVC is supported)
  // returns the instruction at the specified address, given the current
  // RVC mode.  func is set to a pointer to a function that knows how to
  // execute the returned instruction.
  inline insn_fetch_t load_insn(reg_t addr, bool rvc)
  {
    #ifdef RISCV_ENABLE_RVC
    if(addr % 4 == 2 && rvc) // fetch across word boundary
    {
      void* addr_lo = translate(addr, 2, false, true);
      insn_fetch_t fetch;
      fetch.insn.bits = *(uint16_t*)addr_lo;
      size_t dispatch_idx = fetch.insn.bits % processor_t::DISPATCH_TABLE_SIZE;
      fetch.func = processor_t::dispatch_table[dispatch_idx];

      if(!INSN_IS_RVC(fetch.insn.bits))
      {
        void* addr_hi = translate(addr+2, 2, false, true);
        fetch.insn.bits |= (uint32_t)*(uint16_t*)addr_hi << 16;
      }
      return fetch;
    }
    else
    #endif
    {
      reg_t idx = (addr/sizeof(insn_t)) % ICACHE_ENTRIES;
      insn_fetch_t fetch;
      if (unlikely(icache_tag[idx] != addr))
      {
        void* paddr = translate(addr, sizeof(insn_t), false, true);
        fetch.insn = *(insn_t*)paddr;
        size_t dispatch_idx = fetch.insn.bits % processor_t::DISPATCH_TABLE_SIZE;
        fetch.func = processor_t::dispatch_table[dispatch_idx];

        reg_t idx = ((uintptr_t)paddr/sizeof(insn_t)) % ICACHE_ENTRIES;
        icache_tag[idx] = addr;
        icache_data[idx] = fetch.insn;
        icache_func[idx] = fetch.func;

        if (tracer.interested_in_range(addr, addr + sizeof(insn_t), false, true))
          icache_tag[idx] = -1;
      }
      fetch.insn = icache_data[idx];;
      fetch.func = icache_func[idx];
      return fetch;
    }
  }

  // get the virtual address that caused a fault
  reg_t get_badvaddr() { return badvaddr; }

  // get/set the page table base register
  reg_t get_ptbr() { return ptbr; }
  void set_ptbr(reg_t addr) { ptbr = addr & ~(PGSIZE-1); flush_tlb(); }

  // keep the MMU in sync with processor mode
  void set_supervisor(bool sup) { supervisor = sup; }
  void set_vm_enabled(bool en) { vm_enabled = en; }

  // flush the TLB and instruction cache
  void flush_tlb();
  void flush_icache();

  void register_memtracer(memtracer_t*);

private:
  char* mem;
  size_t memsz;
  reg_t badvaddr;
  reg_t ptbr;
  bool supervisor;
  bool vm_enabled;
  memtracer_list_t tracer;

  // implement a TLB for simulator performance
  static const reg_t TLB_ENTRIES = 256;
  char* tlb_data[TLB_ENTRIES];
  reg_t tlb_insn_tag[TLB_ENTRIES];
  reg_t tlb_load_tag[TLB_ENTRIES];
  reg_t tlb_store_tag[TLB_ENTRIES];

  // implement an instruction cache for simulator performance
  static const reg_t ICACHE_ENTRIES = 256;
  insn_t icache_data[ICACHE_ENTRIES];
  insn_func_t icache_func[ICACHE_ENTRIES];
  reg_t icache_tag[ICACHE_ENTRIES];

  // finish translation on a TLB miss and upate the TLB
  void* refill_tlb(reg_t addr, reg_t bytes, bool store, bool fetch);

  // perform a page table walk for a given virtual address
  pte_t walk(reg_t addr);

  // translate a virtual address to a physical address
  void* translate(reg_t addr, reg_t bytes, bool store, bool fetch)
  {
    reg_t idx = (addr >> PGSHIFT) % TLB_ENTRIES;

    reg_t* tlb_tag = fetch ? tlb_insn_tag : store ? tlb_store_tag :tlb_load_tag;
    reg_t expected_tag = addr & ~(PGSIZE-1);
    if(likely(tlb_tag[idx] == expected_tag))
      return ((uintptr_t)addr & (PGSIZE-1)) + tlb_data[idx];

    return refill_tlb(addr, bytes, store, fetch);
  }
  
  friend class processor_t;
};

#endif
