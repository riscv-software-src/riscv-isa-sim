#ifndef _RISCV_MMU_H
#define _RISCV_MMU_H

#include "decode.h"
#include "trap.h"
#include "icsim.h"
#include "common.h"

class processor_t;

typedef reg_t pte_t;

const reg_t LEVELS = 4;
const reg_t PGSHIFT = 12;
const reg_t PGSIZE = 1 << PGSHIFT;
const reg_t PTIDXBITS = PGSHIFT - (sizeof(pte_t) == 8 ? 3 : 2);
const reg_t PPN_BITS = 8*sizeof(reg_t) - PGSHIFT;

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
#define PTE_PERM_SHIFT 4
#define PTE_PPN_SHIFT  12

class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz);
  ~mmu_t();

  #ifdef RISCV_ENABLE_ICSIM
  # define dcsim_tick(dcsim, dtlbsim, addr, size, st) \
      do { if(dcsim) (dcsim)->tick(addr, size, st); \
           if(dtlbsim) (dtlbsim)->tick(addr, sizeof(reg_t), false); } while(0)
  #else
  # define dcsim_tick(dcsim, dtlbsim, addr, size, st)
  #endif

  #define load_func(type) \
    type##_t load_##type(reg_t addr) { \
      if(unlikely(addr % sizeof(type##_t))) \
      { \
        badvaddr = addr; \
        throw trap_load_address_misaligned; \
      } \
      addr = translate(addr, false, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), false); \
      return *(type##_t*)(mem+addr); \
    }

  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      if(unlikely(addr % sizeof(type##_t))) \
      { \
        badvaddr = addr; \
        throw trap_store_address_misaligned; \
      } \
      addr = translate(addr, true, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), true); \
      *(type##_t*)(mem+addr) = val; \
    }

  insn_t __attribute__((always_inline)) load_insn(reg_t addr, bool rvc)
  {
    insn_t insn;

    #ifdef RISCV_ENABLE_RVC
    if(addr % 4 == 2 && rvc) // fetch across word boundary
    {
      reg_t paddr_lo = translate(addr, false, true);
      insn.bits = *(uint16_t*)(mem+paddr_lo);

      if(!INSN_IS_RVC(insn.bits))
      {
        reg_t paddr_hi = translate(addr+2, false, true);
        insn.bits |= (uint32_t)*(uint16_t*)(mem+paddr_hi) << 16;
      }
    }
    else
    #endif
    {
      reg_t idx = (addr/sizeof(insn_t)) % ICACHE_ENTRIES;
      bool hit = icache_tag[idx] == addr;
      if(likely(hit))
        return icache_data[idx];

      // the processor guarantees alignment based upon rvc mode
      reg_t paddr = translate(addr, false, true);
      insn = *(insn_t*)(mem+paddr);

      icache_tag[idx] = addr;
      icache_data[idx] = insn;
    }

    #ifdef RISCV_ENABLE_ICSIM
    if(icsim)
      icsim->tick(addr, insn_length(insn.bits), false);
    if(itlbsim)
      itlbsim->tick(addr, sizeof(reg_t), false);
    #endif

    return insn;
  }

  load_func(uint8)
  load_func(uint16)
  load_func(uint32)
  load_func(uint64)

  load_func(int8)
  load_func(int16)
  load_func(int32)
  load_func(int64)

  store_func(uint8)
  store_func(uint16)
  store_func(uint32)
  store_func(uint64)

  reg_t get_badvaddr() { return badvaddr; }
  reg_t get_ptbr() { return ptbr; }

  void set_supervisor(bool sup) { supervisor = sup; }
  void set_vm_enabled(bool en) { vm_enabled = en; }
  void set_ptbr(reg_t addr) { ptbr = addr & ~(PGSIZE-1); flush_tlb(); }

  void set_icsim(icsim_t* _icsim) { icsim = _icsim; }
  void set_dcsim(icsim_t* _dcsim) { dcsim = _dcsim; }
  void set_itlbsim(icsim_t* _itlbsim) { itlbsim = _itlbsim; }
  void set_dtlbsim(icsim_t* _dtlbsim) { dtlbsim = _dtlbsim; }

  void flush_tlb();
  void flush_icache();

private:
  char* mem;
  size_t memsz;
  reg_t badvaddr;

  reg_t ptbr;
  bool supervisor;
  bool vm_enabled;

  static const reg_t TLB_ENTRIES = 256;
  pte_t tlb_data[TLB_ENTRIES];
  reg_t tlb_insn_tag[TLB_ENTRIES];
  reg_t tlb_load_tag[TLB_ENTRIES];
  reg_t tlb_store_tag[TLB_ENTRIES];

  static const reg_t ICACHE_ENTRIES = 256;
  insn_t icache_data[ICACHE_ENTRIES];
  reg_t icache_tag[ICACHE_ENTRIES];

  icsim_t* icsim;
  icsim_t* dcsim;
  icsim_t* itlbsim;
  icsim_t* dtlbsim;

  reg_t refill(reg_t addr, bool store, bool fetch);
  pte_t walk(reg_t addr);

  reg_t translate(reg_t addr, bool store, bool fetch)
  {
    reg_t idx = (addr >> PGSHIFT) % TLB_ENTRIES;

    reg_t* tlb_tag = fetch ? tlb_insn_tag : store ? tlb_store_tag :tlb_load_tag;
    reg_t expected_tag = addr & ~(PGSIZE-1);
    if(likely(tlb_tag[idx] == expected_tag))
      return (addr & (PGSIZE-1)) | tlb_data[idx];

    return refill(addr, store, fetch);
  }
  
  friend class processor_t;
};

#endif
