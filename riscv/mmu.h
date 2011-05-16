#include "decode.h"
#include "trap.h"
#include "icsim.h"
#include <assert.h>

class processor_t;

const reg_t LEVELS = 4;
const reg_t PGSHIFT = 12;
const reg_t PGSIZE = 1 << PGSHIFT;
const reg_t PPN_BITS = 8*sizeof(reg_t) - PGSHIFT;

struct pte_t
{
  reg_t v  : 1;
  reg_t e  : 1;
  reg_t r  : 1;
  reg_t d  : 1;
  reg_t ux : 1;
  reg_t ur : 1;
  reg_t uw : 1;
  reg_t sx : 1;
  reg_t sr : 1;
  reg_t sw : 1;
  reg_t unused1 : 2;
  reg_t ppn : PPN_BITS;
};

class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz)
   : mem(_mem), memsz(_memsz), badvaddr(0),
     ptbr(0), supervisor(true), vm_enabled(false),
     icsim(NULL), dcsim(NULL), itlbsim(NULL), dtlbsim(NULL)
  {
  }

  #ifdef RISCV_ENABLE_ICSIM
  # define dcsim_tick(dcsim, dtlbsim, addr, size, st) \
      do { if(dcsim) (dcsim)->tick(addr, size, st); \
           if(dtlbsim) (dtlbsim)->tick(addr, sizeof(reg_t), false); } while(0)
  #else
  # define dcsim_tick(dcsim, dtlbsim, addr, size, st)
  #endif

  #define load_func(type) \
    type##_t load_##type(reg_t addr) { \
      check_align(addr, sizeof(type##_t), false, false); \
      addr = translate(addr, false, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), false); \
      return *(type##_t*)(mem+addr); \
    }

  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      check_align(addr, sizeof(type##_t), true, false); \
      addr = translate(addr, true, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), true); \
      *(type##_t*)(mem+addr) = val; \
    }

  insn_t load_insn(reg_t addr, bool rvc)
  {
    insn_t insn;

    reg_t idx = (addr/sizeof(insn_t)) % ICACHE_ENTRIES;
    if(addr % 4 == 0 && icache_tag[idx] == (addr | 1))
      return icache_data[idx];

    #ifdef RISCV_ENABLE_RVC
    if(addr % 4 == 2 && rvc)
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
      check_align(addr, 4, false, true);
      reg_t paddr = translate(addr, false, true);
      insn = *(insn_t*)(mem+paddr);

      icache_tag[idx] = addr | 1;
      icache_data[idx] = insn;
    }

    #ifdef RISCV_ENABLE_ICSIM
    if(icsim)
      icsim->tick(addr, insn_length(insn), false);
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

  static const reg_t TLB_ENTRIES = 32;
  pte_t tlb_data[TLB_ENTRIES];
  reg_t tlb_tag[TLB_ENTRIES];

  static const reg_t ICACHE_ENTRIES = 32;
  insn_t icache_data[ICACHE_ENTRIES];
  reg_t icache_tag[ICACHE_ENTRIES];

  icsim_t* icsim;
  icsim_t* dcsim;
  icsim_t* itlbsim;
  icsim_t* dtlbsim;

  void check_align(reg_t addr, int size, bool store, bool fetch)
  {
    if(addr & (size-1))
    {
      badvaddr = addr;
      if(fetch)
        throw trap_instruction_address_misaligned;
      if(store)
        throw trap_store_address_misaligned;
      throw trap_load_address_misaligned;
    }
  }

  reg_t translate(reg_t addr, bool store, bool fetch)
  {
    reg_t idx = (addr >> PGSHIFT) % TLB_ENTRIES;
    pte_t pte = tlb_data[idx];
    reg_t tag = tlb_tag[idx];

    trap_t trap = store ? trap_store_access_fault
                : fetch ? trap_instruction_access_fault
                :         trap_load_access_fault;

    if(!pte.v || tag != (addr >> PGSHIFT))
    {
      pte = walk(addr);
      if(!pte.v)
        throw trap;

      tlb_data[idx] = pte;
      tlb_tag[idx] = addr >> PGSHIFT;
    }

    if(store && !(supervisor ? pte.sw : pte.uw) ||
       !store && !fetch && !(supervisor ? pte.sr : pte.ur) ||
       !store && !fetch && !(supervisor ? pte.sr : pte.ur))
      throw trap;

    return (addr & (PGSIZE-1)) | (pte.ppn << PGSHIFT);
  }

  pte_t walk(reg_t addr)
  {
    pte_t pte;
  
    if(!vm_enabled)
    {
      pte.v = addr < memsz;
      pte.e = 1;
      pte.r = pte.d = 0;
      pte.ur = pte.uw = pte.ux = pte.sr = pte.sw = pte.sx = 1;
      pte.ppn = addr >> PGSHIFT;
    }
    else
    {
      pte.v = 0;
  
      int lg_ptesz = sizeof(pte_t) == 4 ? 2
                   : sizeof(pte_t) == 8 ? 3
                   : 0;
      assert(lg_ptesz);
  
      reg_t base = ptbr;
  
      for(int i = LEVELS-1; i >= 0; i++)
      {
        reg_t idx = addr >> (PGSHIFT + i*(PGSHIFT - lg_ptesz));
        idx &= (1<<(PGSHIFT - lg_ptesz)) - 1;
  
        reg_t pte_addr = base + idx*sizeof(pte_t);
        if(pte_addr >= memsz)
          break;
  
        pte = *(pte_t*)(mem+pte_addr);
        if(!pte.v || pte.e)
          break;
  
        base = pte.ppn << PGSHIFT;
      }
      pte.v &= pte.e;
    }
  
    return pte;
  }
  
  friend class processor_t;
};
