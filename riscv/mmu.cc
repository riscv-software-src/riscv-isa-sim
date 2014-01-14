// See LICENSE for license details.

#include "mmu.h"
#include "sim.h"
#include "processor.h"

mmu_t::mmu_t(char* _mem, size_t _memsz)
 : mem(_mem), memsz(_memsz), proc(NULL)
{
  flush_tlb();
}

mmu_t::~mmu_t()
{
}

void mmu_t::flush_icache()
{
  for (size_t i = 0; i < ICACHE_SIZE; i++)
    icache[i].tag = -1;
}

void mmu_t::flush_tlb()
{
  memset(tlb_insn_tag, -1, sizeof(tlb_insn_tag));
  memset(tlb_load_tag, -1, sizeof(tlb_load_tag));
  memset(tlb_store_tag, -1, sizeof(tlb_store_tag));

  flush_icache();
}

void* mmu_t::refill_tlb(reg_t addr, reg_t bytes, bool store, bool fetch)
{
  reg_t idx = (addr >> PGSHIFT) % TLB_ENTRIES;
  reg_t expected_tag = addr >> PGSHIFT;

  reg_t pte = walk(addr);

  reg_t pte_perm = pte & PTE_PERM;
  if (proc == NULL || (proc->state.sr & SR_S))
    pte_perm = (pte_perm/(PTE_SX/PTE_UX)) & PTE_PERM;
  pte_perm |= pte & PTE_V;

  reg_t perm = (fetch ? PTE_UX : store ? PTE_UW : PTE_UR) | PTE_V;
  if(unlikely((pte_perm & perm) != perm))
  {
    if (fetch)
      throw trap_instruction_access_fault();

    if (store)
      throw trap_store_access_fault(addr);
    throw trap_load_access_fault(addr);
  }

  reg_t pgoff = addr & (PGSIZE-1);
  reg_t pgbase = pte >> PGSHIFT << PGSHIFT;
  reg_t paddr = pgbase + pgoff;

  if (unlikely(tracer.interested_in_range(pgbase, pgbase + PGSIZE, store, fetch)))
    tracer.trace(paddr, bytes, store, fetch);
  else
  {
    tlb_load_tag[idx] = (pte_perm & PTE_UR) ? expected_tag : -1;
    tlb_store_tag[idx] = (pte_perm & PTE_UW) ? expected_tag : -1;
    tlb_insn_tag[idx] = (pte_perm & PTE_UX) ? expected_tag : -1;
    tlb_data[idx] = mem + pgbase - (addr & ~(PGSIZE-1));
  }

  return mem + paddr;
}

pte_t mmu_t::walk(reg_t addr)
{
  pte_t pte = 0;

  // the address must be a canonical sign-extended VA_BITS-bit number
  int shift = 8*sizeof(reg_t) - VA_BITS;
  if (((sreg_t)addr << shift >> shift) != (sreg_t)addr)
    ;
  else if (proc == NULL || !(proc->state.sr & SR_VM))
  {
    if(addr < memsz)
      pte = PTE_V | PTE_PERM | ((addr >> PGSHIFT) << PGSHIFT);
  }
  else
  {
    reg_t base = proc->get_state()->ptbr;
    reg_t ptd;

    int ptshift = (LEVELS-1)*PTIDXBITS;
    for(reg_t i = 0; i < LEVELS; i++, ptshift -= PTIDXBITS)
    {
      reg_t idx = (addr >> (PGSHIFT+ptshift)) & ((1<<PTIDXBITS)-1);

      reg_t pte_addr = base + idx*sizeof(pte_t);
      if(pte_addr >= memsz)
        break;

      ptd = *(pte_t*)(mem+pte_addr);

      if (!(ptd & PTE_V)) // invalid mapping
        break;
      else if (ptd & PTE_T) // next level of page table
        base = (ptd >> PGSHIFT) << PGSHIFT;
      else // the actual PTE
      {
        // if this PTE is from a larger PT, fake a leaf
        // PTE so the TLB will work right
        reg_t vpn = addr >> PGSHIFT;
        ptd |= (vpn & ((1<<(ptshift))-1)) << PGSHIFT;

        // fault if physical addr is out of range
        if (((ptd >> PGSHIFT) << PGSHIFT) < memsz)
          pte = ptd;
        break;
      }
    }
  }

  return pte;
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}
