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
  for (size_t i = 0; i < ICACHE_ENTRIES; i++)
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

  reg_t pte = 0;
  reg_t mstatus = proc ? proc->state.mstatus : 0;
  
  bool vm_disabled = get_field(mstatus, MSTATUS_VM) == VM_MBARE;
  bool mode_m = get_field(mstatus, MSTATUS_PRV) == PRV_M;
  bool mode_s = get_field(mstatus, MSTATUS_PRV) == PRV_S;
  bool mprv_m = get_field(mstatus, MSTATUS_MPRV) == PRV_M;
  bool mprv_s = get_field(mstatus, MSTATUS_MPRV) == PRV_S;

  if (vm_disabled || (mode_m && (mprv_m || fetch))) {
    // virtual memory is disabled.  merely check legality of physical address.
    if (addr < memsz) {
      // produce a fake PTE for the TLB's benefit.
      pte = PTE_V | PTE_UX | PTE_SX | ((addr >> PGSHIFT) << PGSHIFT);
      if (vm_disabled || !(mode_m && !mprv_m))
        pte |= PTE_UR | PTE_SR | PTE_UW | PTE_SW;
    }
  } else {
    pte = walk(addr, store);
  }

  reg_t pte_perm = pte & PTE_PERM;
  if (mode_s || (mode_m && mprv_s && !fetch))
    pte_perm = (pte_perm/(PTE_SX/PTE_UX)) & PTE_PERM;
  pte_perm |= pte & PTE_V;

  reg_t perm = (fetch ? PTE_UX : store ? PTE_UW : PTE_UR) | PTE_V;
  if(unlikely((pte_perm & perm) != perm))
  {
    if (fetch)
      throw trap_instruction_access_fault(addr);
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
    tlb_store_tag[idx] = (pte_perm & PTE_UW) && store ? expected_tag : -1;
    tlb_insn_tag[idx] = (pte_perm & PTE_UX) ? expected_tag : -1;
    tlb_data[idx] = mem + pgbase - (addr & ~(PGSIZE-1));
  }

  return mem + paddr;
}

pte_t mmu_t::walk(reg_t addr, bool store)
{
  reg_t msb_mask = -(reg_t(1) << (VA_BITS-1));
  if ((addr & msb_mask) != 0 && (addr & msb_mask) != msb_mask)
    return 0; // address isn't properly sign-extended

  reg_t base = proc->get_state()->sptbr;

  int ptshift = (LEVELS-1)*PTIDXBITS;
  for (reg_t i = 0; i < LEVELS; i++, ptshift -= PTIDXBITS) {
    reg_t idx = (addr >> (PGSHIFT+ptshift)) & ((1<<PTIDXBITS)-1);

    // check that physical address of PTE is legal
    reg_t pte_addr = base + idx*sizeof(pte_t);
    if (pte_addr >= memsz)
      return 0;

    pte_t* ppte = (pte_t*)(mem+pte_addr);

    if (!(*ppte & PTE_V)) { // invalid mapping
      return 0;
    } else if (*ppte & PTE_T) { // next level of page table
      base = (*ppte >> PGSHIFT) << PGSHIFT;
    } else {
      // we've found the PTE.  set referenced and possibly dirty bits.
      *ppte |= PTE_R | (store ? PTE_D : 0);
      // for superpage mappings, make a fake leaf PTE for the TLB's benefit.
      reg_t vpn = addr >> PGSHIFT;
      reg_t pte = *ppte | ((vpn & ((1<<(ptshift))-1)) << PGSHIFT);

      // check that physical address is legal
      if (((pte >> PGSHIFT) << PGSHIFT) >= memsz)
        return 0;

      return pte;
    }
  }
  return 0;
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}
