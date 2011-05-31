#include "decode.h"
#include "trap.h"
#include "icsim.h"
#include "common.h"
#include <assert.h>

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
      if(unlikely(addr % sizeof(type##_t))) \
        throw trap_load_address_misaligned; \
      addr = translate(addr, false, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), false); \
      return *(type##_t*)(mem+addr); \
    }

  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      if(unlikely(addr % sizeof(type##_t))) \
        throw trap_store_address_misaligned; \
      addr = translate(addr, true, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), true); \
      *(type##_t*)(mem+addr) = val; \
    }

  insn_t __attribute__((always_inline)) load_insn(reg_t addr, bool rvc)
  {
    insn_t insn;

    reg_t idx = (addr/sizeof(insn_t)) % ICACHE_ENTRIES;
    bool hit = addr % 4 == 0 && icache_tag[idx] == (addr | 1);
    if(likely(hit))
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
      if(unlikely(addr % 4))
        throw trap_instruction_address_misaligned;
      reg_t paddr = translate(addr, false, true);
      insn = *(insn_t*)(mem+paddr);

      icache_tag[idx] = addr | 1;
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
  reg_t tlb_tag[TLB_ENTRIES];

  static const reg_t ICACHE_ENTRIES = 256;
  insn_t icache_data[ICACHE_ENTRIES];
  reg_t icache_tag[ICACHE_ENTRIES];

  icsim_t* icsim;
  icsim_t* dcsim;
  icsim_t* itlbsim;
  icsim_t* dtlbsim;

  reg_t translate(reg_t addr, bool store, bool fetch)
  {
    reg_t idx = (addr >> PGSHIFT) % TLB_ENTRIES;
    pte_t pte = tlb_data[idx];
    reg_t tag = tlb_tag[idx];

    trap_t trap = store ? trap_store_access_fault
                : fetch ? trap_instruction_access_fault
                :         trap_load_access_fault;

    bool hit = (pte & PTE_E) && tag == (addr >> PGSHIFT);
    if(unlikely(!hit))
    {
      pte = walk(addr);
      if(!(pte & PTE_E))
        throw trap;

      tlb_data[idx] = pte;
      tlb_tag[idx] = addr >> PGSHIFT;
    }

    reg_t access_type = store ? PTE_UW : fetch ? PTE_UX : PTE_UR;
    if(supervisor)
      access_type <<= 3;
    if(unlikely(!(access_type & pte & PTE_PERM)))
      throw trap;

    return (addr & (PGSIZE-1)) | ((pte >> PTE_PPN_SHIFT) << PGSHIFT);
  }

  pte_t walk(reg_t addr)
  {
    pte_t pte = 0;
  
    if(!vm_enabled)
    {
      if(addr < memsz)
        pte = PTE_E | PTE_PERM | ((addr >> PGSHIFT) << PTE_PPN_SHIFT);
    }
    else
    {
      reg_t base = ptbr;
      reg_t ptd;
  
      int ptshift = (LEVELS-1)*PTIDXBITS;
      for(reg_t i = 0; i < LEVELS; i++, ptshift -= PTIDXBITS)
      {
        reg_t idx = (addr >> (PGSHIFT+ptshift)) & ((1<<PTIDXBITS)-1);
  
        reg_t pte_addr = base + idx*sizeof(pte_t);
        if(pte_addr >= memsz)
          break;
  
        ptd = *(pte_t*)(mem+pte_addr);
        if(ptd & PTE_E)
        {
          // if this PTE is from a larger PT, fake a leaf
          // PTE so the TLB will work right
          reg_t vpn = addr >> PGSHIFT;
          pte |= ptd | (vpn & ((1<<(ptshift))-1)) << PTE_PPN_SHIFT;
          break;
        }
        else if(!(ptd & PTE_T))
          break;
  
        base = (ptd >> PTE_PPN_SHIFT) << PGSHIFT;
      }
    }
  
    return pte;
  }
  
  friend class processor_t;
};
