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

reg_t mmu_t::translate(reg_t addr, access_type type)
{
  if (!proc)
    return addr;

  reg_t mode = get_field(proc->state.mstatus, MSTATUS_PRV);
  if (type != FETCH && get_field(proc->state.mstatus, MSTATUS_MPRV))
    mode = get_field(proc->state.mstatus, MSTATUS_PRV1);
  if (get_field(proc->state.mstatus, MSTATUS_VM) == VM_MBARE)
    mode = PRV_M;

  if (mode == PRV_M) {
    reg_t msb_mask = (reg_t(2) << (proc->xlen-1))-1; // zero-extend from xlen
    return addr & msb_mask;
  }
  return walk(addr, mode > PRV_U, type) | (addr & (PGSIZE-1));
}

const uint16_t* mmu_t::fetch_slow_path(reg_t addr)
{
  reg_t paddr = translate(addr, FETCH);
  if (paddr >= memsz)
    throw trap_instruction_access_fault(addr);
  return (const uint16_t*)(mem + paddr);
}

void mmu_t::load_slow_path(reg_t addr, reg_t len, uint8_t* bytes)
{
  reg_t paddr = translate(addr, LOAD);
  if (paddr < memsz) {
    memcpy(bytes, mem + paddr, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, LOAD))
      tracer.trace(paddr, len, LOAD);
    else
      refill_tlb(addr, paddr, LOAD);
  } else if (!proc || !proc->sim->mmio_load(addr, len, bytes)) {
    throw trap_load_access_fault(addr);
  }
}

void mmu_t::store_slow_path(reg_t addr, reg_t len, const uint8_t* bytes)
{
  reg_t paddr = translate(addr, STORE);
  if (paddr < memsz) {
    memcpy(mem + paddr, bytes, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, STORE))
      tracer.trace(paddr, len, STORE);
    else
      refill_tlb(addr, paddr, STORE);
  } else if (!proc || !proc->sim->mmio_store(addr, len, bytes)) {
    throw trap_store_access_fault(addr);
  }
}

void mmu_t::refill_tlb(reg_t vaddr, reg_t paddr, access_type type)
{
  reg_t idx = (vaddr >> PGSHIFT) % TLB_ENTRIES;
  reg_t expected_tag = vaddr >> PGSHIFT;

  if (tlb_load_tag[idx] != expected_tag) tlb_load_tag[idx] = -1;
  if (tlb_store_tag[idx] != expected_tag) tlb_store_tag[idx] = -1;
  if (tlb_insn_tag[idx] != expected_tag) tlb_insn_tag[idx] = -1;

  if (type == FETCH) tlb_insn_tag[idx] = expected_tag;
  else if (type == STORE) tlb_store_tag[idx] = expected_tag;
  else tlb_load_tag[idx] = expected_tag;

  tlb_data[idx] = mem + paddr - vaddr;
}

reg_t mmu_t::walk(reg_t addr, bool supervisor, access_type type)
{
  int levels, ptidxbits, ptesize;
  switch (get_field(proc->get_state()->mstatus, MSTATUS_VM))
  {
    case VM_SV32: levels = 2; ptidxbits = 10; ptesize = 4; break;
    case VM_SV39: levels = 3; ptidxbits = 9; ptesize = 8; break;
    case VM_SV48: levels = 4; ptidxbits = 9; ptesize = 8; break;
    default: abort();
  }

  // verify bits xlen-1:va_bits-1 are all equal
  int va_bits = PGSHIFT + levels * ptidxbits;
  reg_t mask = (reg_t(1) << (proc->xlen - (va_bits-1))) - 1;
  reg_t masked_msbs = (addr >> (va_bits-1)) & mask;
  if (masked_msbs != 0 && masked_msbs != mask)
    return -1;

  reg_t base = proc->get_state()->sptbr;
  int ptshift = (levels - 1) * ptidxbits;
  for (int i = 0; i < levels; i++, ptshift -= ptidxbits) {
    reg_t idx = (addr >> (PGSHIFT + ptshift)) & ((1 << ptidxbits) - 1);

    // check that physical address of PTE is legal
    reg_t pte_addr = base + idx * ptesize;
    if (pte_addr >= memsz)
      break;

    void* ppte = mem + pte_addr;
    reg_t pte = ptesize == 4 ? *(uint32_t*)ppte : *(uint64_t*)ppte;
    reg_t ppn = pte >> PTE_PPN_SHIFT;

    if (PTE_TABLE(pte)) { // next level of page table
      base = ppn << PGSHIFT;
    } else if (!PTE_CHECK_PERM(pte, supervisor, type == STORE, type == FETCH)) {
      break;
    } else {
      // set referenced and possibly dirty bits.
      *(uint32_t*)ppte |= PTE_R | ((type == STORE) * PTE_D);
      // for superpage mappings, make a fake leaf PTE for the TLB's benefit.
      reg_t vpn = addr >> PGSHIFT;
      return (ppn | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;
    }
  }

  return -1;
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}
