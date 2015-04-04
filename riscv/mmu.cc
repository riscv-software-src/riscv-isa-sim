// See LICENSE for license details.

#include "mmu.h"
#include "sim.h"
#include "processor.h"

#define LEVELS(xlen) ((xlen) == 32 ? 2 : 3)
#define PPN_SHIFT(xlen) ((xlen) == 32 ? 10 : 26)
#define PTIDXBITS(xlen) ((xlen) == 32 ? 10 : 9)
#define VPN_BITS(xlen) (PTIDXBITS(xlen) * LEVELS(xlen))
#define VA_BITS(xlen) (VPN_BITS(xlen) + PGSHIFT)

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

  reg_t mstatus = proc ? proc->state.mstatus : 0;
  
  bool vm_disabled = get_field(mstatus, MSTATUS_VM) == VM_MBARE;
  bool mode_m = get_field(mstatus, MSTATUS_PRV) == PRV_M;
  bool mode_s = get_field(mstatus, MSTATUS_PRV) == PRV_S;
  bool mprv_m = get_field(mstatus, MSTATUS_MPRV) == PRV_M;
  bool mprv_s = get_field(mstatus, MSTATUS_MPRV) == PRV_S;

  reg_t pgbase;
  if (vm_disabled || (mode_m && (mprv_m || fetch))) {
    pgbase = addr & -PGSIZE;
    // virtual memory is disabled.  merely check legality of physical address.
    if (addr >= memsz)
      pgbase = -1;
  } else {
    pgbase = walk(addr, mode_s || (mode_m && mprv_s), store, fetch);
  }

  reg_t pgoff = addr & (PGSIZE-1);
  reg_t paddr = pgbase + pgoff;

  if (pgbase == reg_t(-1)) {
    if (fetch) throw trap_instruction_access_fault(addr);
    else if (store) throw trap_store_access_fault(addr);
    else throw trap_load_access_fault(addr);
  }

  if (unlikely(tracer.interested_in_range(pgbase, pgbase + PGSIZE, store, fetch)))
    tracer.trace(paddr, bytes, store, fetch);
  else
  {
    if (tlb_load_tag[idx] != expected_tag) tlb_load_tag[idx] = -1;
    if (tlb_store_tag[idx] != expected_tag) tlb_store_tag[idx] = -1;
    if (tlb_insn_tag[idx] != expected_tag) tlb_insn_tag[idx] = -1;

    if (fetch) tlb_insn_tag[idx] = expected_tag;
    else if (store) tlb_store_tag[idx] = expected_tag;
    else tlb_load_tag[idx] = expected_tag;

    tlb_data[idx] = mem + pgbase - (addr & ~(PGSIZE-1));
  }

  return mem + paddr;
}

reg_t mmu_t::walk(reg_t addr, bool supervisor, bool store, bool fetch)
{
  reg_t msb_mask = -(reg_t(1) << (VA_BITS(proc->xlen) - 1));
  if ((addr & msb_mask) != 0 && (addr & msb_mask) != msb_mask)
    return -1; // address isn't properly sign-extended

  reg_t base = proc->get_state()->sptbr;

  int xlen = proc->max_xlen;
  int ptshift = (LEVELS(xlen) - 1) * PTIDXBITS(xlen);
  for (reg_t i = 0; i < LEVELS(xlen); i++, ptshift -= PTIDXBITS(xlen)) {
    reg_t idx = (addr >> (PGSHIFT+ptshift)) & ((1<<PTIDXBITS(xlen))-1);

    // check that physical address of PTE is legal
    reg_t pte_addr = base + idx*sizeof(reg_t);
    if (pte_addr >= memsz)
      return -1;

    reg_t* ppte = (reg_t*)(mem+pte_addr);
    reg_t ppn = *ppte >> PPN_SHIFT(xlen);

    if ((*ppte & PTE_TYPE) == PTE_TYPE_TABLE) { // next level of page table
      base = ppn << PGSHIFT;
    } else {
      // we've found the PTE. check the permissions.
      if (!PTE_CHECK_PERM(*ppte, supervisor, store, fetch))
        return -1;
      // set referenced and possibly dirty bits.
      *ppte |= PTE_R;
      if (store)
        *ppte |= PTE_D;
      // for superpage mappings, make a fake leaf PTE for the TLB's benefit.
      reg_t vpn = addr >> PGSHIFT;
      reg_t addr = (ppn | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;

      // check that physical address is legal
      if (addr >= memsz)
        return -1;

      return addr;
    }
  }

  return -1;
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}
