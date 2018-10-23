// See LICENSE for license details.

#include "mmu.h"
#include "simif.h"
#include "processor.h"

mmu_t::mmu_t(simif_t* sim, processor_t* proc)
 : sim(sim), proc(proc),
  check_triggers_fetch(false),
  check_triggers_load(false),
  check_triggers_store(false),
  matched_trigger(NULL)
{
  flush_tlb();
  yield_load_reservation();
}

mmu_t::~mmu_t()
{
#ifdef RISCV_ENABLE_MEMORY_HAZARDS
  if (memhaz_enabled)
  {
    fprintf(stderr, "RAR Address Histogram size: %zu\n", rar_addr_histogram.size());
    //for (auto it : rar_addr_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAR PC Histogram size: %zu\n", rar_pc_histogram.size());
    //for (auto it : rar_pc_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAR-Multi Address Histogram size: %zu\n", rar_multi_addr_histogram.size());
    for (auto it : rar_multi_addr_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAR-Multi PC Histogram size: %zu\n", rar_multi_pc_histogram.size());
    for (auto it : rar_multi_pc_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAW Address Histogram size: %zu\n", raw_addr_histogram.size());
    //for (auto it : raw_addr_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAW PC Histogram size: %zu\n", raw_pc_histogram.size());
    //for (auto it : raw_pc_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAW-Multi Address Histogram size: %zu\n", raw_multi_addr_histogram.size());
    for (auto it : raw_multi_addr_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "RAW-Multi PC Histogram size: %zu\n", raw_multi_pc_histogram.size());
    for (auto it : raw_multi_pc_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAR Address Histogram size: %zu\n", war_addr_histogram.size());
    //for (auto it : war_addr_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAR PC Histogram size: %zu\n", war_pc_histogram.size());
    //for (auto it : war_pc_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAR-Multi Address Histogram size: %zu\n", war_multi_addr_histogram.size());
    for (auto it : war_multi_addr_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAR-Multi PC Histogram size: %zu\n", war_multi_pc_histogram.size());
    for (auto it : war_multi_pc_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAW Address Histogram size: %zu\n", waw_addr_histogram.size());
    //for (auto it : waw_addr_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAW PC Histogram size: %zu\n", waw_pc_histogram.size());
    //for (auto it : waw_pc_histogram)
    //  fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAW-Multi Address Histogram size: %zu\n", waw_multi_addr_histogram.size());
    for (auto it : waw_multi_addr_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "WAW-Multi PC Histogram size: %zu\n", waw_multi_pc_histogram.size());
    for (auto it : waw_multi_pc_histogram)
      fprintf(stderr, "  %0" PRIx64 " %" PRIu64 "\n", it.first, it.second);
    fprintf(stderr, "Loads PKI: %g\n", (1000.0 * minstret_load) / proc->state.minstret);
    fprintf(stderr, "Stores PKI: %g\n", (1000.0 * minstret_store) / proc->state.minstret);
    fprintf(stderr, "Misaligned PKI: %g\n", (1000.0 * (minstret_load_misaligned + minstret_store_misaligned)) / proc->state.minstret);
    fprintf(stderr, "RAR Hazards PKI: %g\n", (1000.0 * minstret_load_rar_hazard) / proc->state.minstret);
    fprintf(stderr, "RAR Multi-Hazards PKI: %g\n", (1000.0 * minstret_load_rar_multi_hazard) / proc->state.minstret);
    fprintf(stderr, "RAW Hazards PKI: %g\n", (1000.0 * minstret_load_raw_hazard) / proc->state.minstret);
    fprintf(stderr, "RAW Multi-Hazards PKI: %g\n", (1000.0 * minstret_load_raw_multi_hazard) / proc->state.minstret);
    fprintf(stderr, "WAR Hazards PKI: %g\n", (1000.0 * minstret_load_war_hazard) / proc->state.minstret);
    fprintf(stderr, "WAR Multi-Hazards PKI: %g\n", (1000.0 * minstret_load_war_multi_hazard) / proc->state.minstret);
    fprintf(stderr, "WAW Hazards PKI: %g\n", (1000.0 * minstret_load_waw_hazard) / proc->state.minstret);
    fprintf(stderr, "WAW Multi-Hazards PKI: %g\n", (1000.0 * minstret_load_waw_multi_hazard) / proc->state.minstret);
  }
#endif
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

  reg_t mode = proc->state.prv;
  if (type != FETCH) {
    if (!proc->state.dcsr.cause && get_field(proc->state.mstatus, MSTATUS_MPRV))
      mode = get_field(proc->state.mstatus, MSTATUS_MPP);
  }

  return walk(addr, type, mode) | (addr & (PGSIZE-1));
}

tlb_entry_t mmu_t::fetch_slow_path(reg_t vaddr)
{
  reg_t paddr = translate(vaddr, FETCH);

  if (auto host_addr = sim->addr_to_mem(paddr)) {
    return refill_tlb(vaddr, paddr, host_addr, FETCH);
  } else {
    if (!sim->mmio_load(paddr, sizeof fetch_temp, (uint8_t*)&fetch_temp))
      throw trap_instruction_access_fault(vaddr);
    tlb_entry_t entry = {(char*)&fetch_temp - vaddr, paddr - vaddr};
    return entry;
  }
}

reg_t reg_from_bytes(size_t len, const uint8_t* bytes)
{
  switch (len) {
    case 1:
      return bytes[0];
    case 2:
      return bytes[0] |
        (((reg_t) bytes[1]) << 8);
    case 4:
      return bytes[0] |
        (((reg_t) bytes[1]) << 8) |
        (((reg_t) bytes[2]) << 16) |
        (((reg_t) bytes[3]) << 24);
    case 8:
      return bytes[0] |
        (((reg_t) bytes[1]) << 8) |
        (((reg_t) bytes[2]) << 16) |
        (((reg_t) bytes[3]) << 24) |
        (((reg_t) bytes[4]) << 32) |
        (((reg_t) bytes[5]) << 40) |
        (((reg_t) bytes[6]) << 48) |
        (((reg_t) bytes[7]) << 56);
  }
  abort();
}

void mmu_t::load_slow_path(reg_t addr, reg_t len, uint8_t* bytes)
{
  reg_t paddr = translate(addr, LOAD);

  if (auto host_addr = sim->addr_to_mem(paddr)) {
    memcpy(bytes, host_addr, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, LOAD))
      tracer.trace(paddr, len, LOAD);
    else
      refill_tlb(addr, paddr, host_addr, LOAD);
  } else if (!sim->mmio_load(paddr, len, bytes)) {
    throw trap_load_access_fault(addr);
  }

  if (!matched_trigger) {
    reg_t data = reg_from_bytes(len, bytes);
    matched_trigger = trigger_exception(OPERATION_LOAD, addr, data);
    if (matched_trigger)
      throw *matched_trigger;
  }
}

void mmu_t::store_slow_path(reg_t addr, reg_t len, const uint8_t* bytes)
{
  reg_t paddr = translate(addr, STORE);

  if (!matched_trigger) {
    reg_t data = reg_from_bytes(len, bytes);
    matched_trigger = trigger_exception(OPERATION_STORE, addr, data);
    if (matched_trigger)
      throw *matched_trigger;
  }

  if (auto host_addr = sim->addr_to_mem(paddr)) {
    memcpy(host_addr, bytes, len);
    if (tracer.interested_in_range(paddr, paddr + PGSIZE, STORE))
      tracer.trace(paddr, len, STORE);
    else
      refill_tlb(addr, paddr, host_addr, STORE);
  } else if (!sim->mmio_store(paddr, len, bytes)) {
    throw trap_store_access_fault(addr);
  }
}

void mmu_t::set_memhaz(bool value)
{
  memhaz_enabled = value;
#ifndef RISCV_ENABLE_MEMORY_HAZARDS
  if (value) {
    fprintf(stderr, "Memory Hazards support has not been properly enabled;");
    fprintf(stderr, " please re-build the riscv-isa-run project using \"configure --enable-memoryhazards\".\n");
  }
#endif
}

void mmu_t::memory_log_hazard(reg_t addr, size_t size, bool store_not_load)
{
#ifdef RISCV_ENABLE_MEMORY_HAZARDS
  if (store_not_load) { minstret_store++; } else { minstret_load++; }
  bool rar_hazard_found = false;
  bool rar_multi_hazard_found = false;
  bool raw_hazard_found = false;
  bool raw_multi_hazard_found = false;
  bool war_hazard_found = false;
  bool war_multi_hazard_found = false;
  bool waw_hazard_found = false;
  bool waw_multi_hazard_found = false;
  reg_t minstret, pc = 0;
  if (proc) {
    minstret = proc->state.minstret;
    pc = proc->state.pc;
  }
  for (reg_t offset=addr; offset<(addr+size); offset++) {  // for each byte addressed
    if (store_not_load) {
      if (war_addrmap.count(offset)) {  // if WAR hazard address map key exists
        if ((minstret - war_addrmap[offset]) < 100) {
          if (war_hazard_found && (war_addrmap[offset]!=war_addrmap[offset-1])) war_multi_hazard_found = true;
          war_hazard_found = true;
        }
      }
      if (waw_addrmap.count(offset)) {  // if WAW hazard address map key exists
        if ((minstret - waw_addrmap[offset]) < 100) {
          if (waw_hazard_found && (waw_addrmap[offset]!=waw_addrmap[offset-1])) waw_multi_hazard_found = true;
          waw_hazard_found = true;
        }
      }
    } else {
      if (rar_addrmap.count(offset)) {  // if RAR hazard address map key exists
        if ((minstret - rar_addrmap[offset]) < 100) {
          if (rar_hazard_found && (rar_addrmap[offset]!=rar_addrmap[offset-1])) rar_multi_hazard_found = true;
          rar_hazard_found = true;
        }
      }
      if (raw_addrmap.count(offset)) {  // if RAW hazard address map key exists
        if ((minstret - raw_addrmap[offset]) < 100) {
          if (raw_hazard_found && (raw_addrmap[offset]!=raw_addrmap[offset-1])) raw_multi_hazard_found = true;
          raw_hazard_found = true;
        }
      }
    }
  }
  if (rar_hazard_found) minstret_load_rar_hazard++;
  if (rar_multi_hazard_found) minstret_load_rar_multi_hazard++;
  if (raw_hazard_found) minstret_load_raw_hazard++;
  if (raw_multi_hazard_found) minstret_load_raw_multi_hazard++;
  if (war_hazard_found) minstret_load_war_hazard++;
  if (war_multi_hazard_found) minstret_load_war_multi_hazard++;
  if (waw_hazard_found) minstret_load_waw_hazard++;
  if (waw_multi_hazard_found) minstret_load_waw_multi_hazard++;
  for (reg_t offset=addr; offset<(addr+size); offset++) {  // for each byte addressed
    if (store_not_load) {
      raw_addrmap[offset] = minstret;
      waw_addrmap[offset] = minstret;
    } else {
      rar_addrmap[offset] = minstret;
      war_addrmap[offset] = minstret;
    }
    if (rar_hazard_found) {
      rar_addr_histogram[offset]++;
      rar_pc_histogram[pc]++;
    }
    if (rar_multi_hazard_found) {
      rar_multi_addr_histogram[offset]++;
      rar_multi_pc_histogram[pc]++;
    }
    if (raw_hazard_found) {
      raw_addr_histogram[offset]++;
      raw_pc_histogram[pc]++;
    }
    if (raw_multi_hazard_found) {
      raw_multi_addr_histogram[offset]++;
      raw_multi_pc_histogram[pc]++;
    }
    if (war_hazard_found) {
      war_addr_histogram[offset]++;
      war_pc_histogram[pc]++;
    }
    if (war_multi_hazard_found) {
      war_multi_addr_histogram[offset]++;
      war_multi_pc_histogram[pc]++;
    }
    if (waw_hazard_found) {
      waw_addr_histogram[offset]++;
      waw_pc_histogram[pc]++;
    }
    if (waw_multi_hazard_found) {
      waw_multi_addr_histogram[offset]++;
      waw_multi_pc_histogram[pc]++;
    }
  }
#endif
}

void mmu_t::memory_log_misaligned(reg_t addr, size_t size, bool store_not_load)
{
#ifdef RISCV_ENABLE_MEMORY_HAZARDS
  if (store_not_load) { minstret_store_misaligned++; } else { minstret_load_misaligned++; }
#endif
}

tlb_entry_t mmu_t::refill_tlb(reg_t vaddr, reg_t paddr, char* host_addr, access_type type)
{
  reg_t idx = (vaddr >> PGSHIFT) % TLB_ENTRIES;
  reg_t expected_tag = vaddr >> PGSHIFT;

  if ((tlb_load_tag[idx] & ~TLB_CHECK_TRIGGERS) != expected_tag)
    tlb_load_tag[idx] = -1;
  if ((tlb_store_tag[idx] & ~TLB_CHECK_TRIGGERS) != expected_tag)
    tlb_store_tag[idx] = -1;
  if ((tlb_insn_tag[idx] & ~TLB_CHECK_TRIGGERS) != expected_tag)
    tlb_insn_tag[idx] = -1;

  if ((check_triggers_fetch && type == FETCH) ||
      (check_triggers_load && type == LOAD) ||
      (check_triggers_store && type == STORE))
    expected_tag |= TLB_CHECK_TRIGGERS;

  if (type == FETCH) tlb_insn_tag[idx] = expected_tag;
  else if (type == STORE) tlb_store_tag[idx] = expected_tag;
  else tlb_load_tag[idx] = expected_tag;

  tlb_entry_t entry = {host_addr - vaddr, paddr - vaddr};
  tlb_data[idx] = entry;
  return entry;
}

reg_t mmu_t::walk(reg_t addr, access_type type, reg_t mode)
{
  vm_info vm = decode_vm_info(proc->max_xlen, mode, proc->get_state()->satp);
  if (vm.levels == 0)
    return addr & ((reg_t(2) << (proc->xlen-1))-1); // zero-extend from xlen

  bool s_mode = mode == PRV_S;
  bool sum = get_field(proc->state.mstatus, MSTATUS_SUM);
  bool mxr = get_field(proc->state.mstatus, MSTATUS_MXR);

  // verify bits xlen-1:va_bits-1 are all equal
  int va_bits = PGSHIFT + vm.levels * vm.idxbits;
  reg_t mask = (reg_t(1) << (proc->xlen - (va_bits-1))) - 1;
  reg_t masked_msbs = (addr >> (va_bits-1)) & mask;
  if (masked_msbs != 0 && masked_msbs != mask)
    vm.levels = 0;

  reg_t base = vm.ptbase;
  for (int i = vm.levels - 1; i >= 0; i--) {
    int ptshift = i * vm.idxbits;
    reg_t idx = (addr >> (PGSHIFT + ptshift)) & ((1 << vm.idxbits) - 1);

    // check that physical address of PTE is legal
    auto ppte = sim->addr_to_mem(base + idx * vm.ptesize);
    if (!ppte)
      goto fail_access;

    reg_t pte = vm.ptesize == 4 ? *(uint32_t*)ppte : *(uint64_t*)ppte;
    reg_t ppn = pte >> PTE_PPN_SHIFT;

    if (PTE_TABLE(pte)) { // next level of page table
      base = ppn << PGSHIFT;
    } else if ((pte & PTE_U) ? s_mode && (type == FETCH || !sum) : !s_mode) {
      break;
    } else if (!(pte & PTE_V) || (!(pte & PTE_R) && (pte & PTE_W))) {
      break;
    } else if (type == FETCH ? !(pte & PTE_X) :
               type == LOAD ?  !(pte & PTE_R) && !(mxr && (pte & PTE_X)) :
                               !((pte & PTE_R) && (pte & PTE_W))) {
      break;
    } else if ((ppn & ((reg_t(1) << ptshift) - 1)) != 0) {
      break;
    } else {
      reg_t ad = PTE_A | ((type == STORE) * PTE_D);
#ifdef RISCV_ENABLE_DIRTY
      // set accessed and possibly dirty bits.
      *(uint32_t*)ppte |= ad;
#else
      // take exception if access or possibly dirty bit is not set.
      if ((pte & ad) != ad)
        break;
#endif
      // for superpage mappings, make a fake leaf PTE for the TLB's benefit.
      reg_t vpn = addr >> PGSHIFT;
      reg_t value = (ppn | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;
      return value;
    }
  }

fail:
  switch (type) {
    case FETCH: throw trap_instruction_page_fault(addr);
    case LOAD: throw trap_load_page_fault(addr);
    case STORE: throw trap_store_page_fault(addr);
    default: abort();
  }

fail_access:
  switch (type) {
    case FETCH: throw trap_instruction_access_fault(addr);
    case LOAD: throw trap_load_access_fault(addr);
    case STORE: throw trap_store_access_fault(addr);
    default: abort();
  }
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}
