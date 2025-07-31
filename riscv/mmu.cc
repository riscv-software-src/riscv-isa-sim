// See LICENSE for license details.

#include "config.h"
#include "mmu.h"
#include "arith.h"
#include "simif.h"
#include "processor.h"
#include "decode_macros.h"

mmu_t::mmu_t(simif_t* sim, endianness_t endianness, processor_t* proc, reg_t cache_blocksz)
 : sim(sim), proc(proc), blocksz(cache_blocksz),
#ifdef RISCV_ENABLE_DUAL_ENDIAN
  target_big_endian(endianness == endianness_big),
#endif
  check_triggers_fetch(false),
  check_triggers_load(false),
  check_triggers_store(false)
{
#ifndef RISCV_ENABLE_DUAL_ENDIAN
  assert(endianness == endianness_little);
#endif
  flush_tlb();
  yield_load_reservation();
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
  memset(tlb_insn, -1, sizeof(tlb_insn));
  memset(tlb_load, -1, sizeof(tlb_load));
  memset(tlb_store, -1, sizeof(tlb_store));

  flush_icache();
}

void throw_access_exception(bool virt, reg_t addr, access_type type)
{
  switch (type) {
    case FETCH: throw trap_instruction_access_fault(virt, addr, 0, 0);
    case LOAD: throw trap_load_access_fault(virt, addr, 0, 0);
    case STORE: throw trap_store_access_fault(virt, addr, 0, 0);
    default: abort();
  }
}

reg_t mmu_t::translate(mem_access_info_t access_info, reg_t len)
{
  reg_t addr = access_info.transformed_vaddr;
  access_type type = access_info.type;
  if (!proc)
    return addr;

  bool virt = access_info.effective_virt;
  reg_t mode = (reg_t) access_info.effective_priv;

  reg_t paddr = walk(access_info) | (addr & (PGSIZE-1));
  if (!pmp_ok(paddr, len, access_info.flags.ss_access ? STORE : type, mode, access_info.flags.hlvx))
    throw_access_exception(virt, addr, access_info.flags.ss_access ? STORE : type);
  return paddr;
}

inline mmu_t::insn_parcel_t mmu_t::perform_intrapage_fetch(reg_t vaddr, uintptr_t host_addr, reg_t paddr)
{
  insn_parcel_t res;

  if (host_addr)
    memcpy(&res, (char*)host_addr, sizeof(res));
  else if (!mmio_fetch(paddr, sizeof(res), (uint8_t*)&res))
    throw trap_instruction_access_fault(proc->state.v, vaddr, 0, 0);

  return res;
}

mmu_t::insn_parcel_t mmu_t::fetch_slow_path(reg_t vaddr)
{
  if (matched_trigger) {
    auto trig = matched_trigger.value();
    matched_trigger.reset();
    throw trig;
  }

  if  (auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_insn, vaddr, TLB_FLAGS & ~TLB_CHECK_TRIGGERS); tlb_hit) {
    // Fast path for simple cases
    return perform_intrapage_fetch(vaddr, host_addr, paddr);
  }

  auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_insn, vaddr, TLB_FLAGS);
  auto access_info = generate_access_info(vaddr, FETCH, {});
  check_triggers(triggers::OPERATION_EXECUTE, vaddr, access_info.effective_virt);

  if (!tlb_hit) {
    paddr = translate(access_info, sizeof(insn_parcel_t));
    host_addr = (uintptr_t)sim->addr_to_mem(paddr);

    refill_tlb(vaddr, paddr, (char*)host_addr, FETCH);
  }

  auto res = perform_intrapage_fetch(vaddr, host_addr, paddr);

  check_triggers(triggers::OPERATION_EXECUTE, vaddr, access_info.effective_virt, from_le(res));

  return res;
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

bool mmu_t::mmio_ok(reg_t paddr, access_type UNUSED type)
{
  // Disallow access to debug region when not in debug mode
  static_assert(DEBUG_START == 0);
  if (/* paddr >= DEBUG_START && */ paddr <= DEBUG_END && proc && !proc->state.debug_mode)
    return false;

  return true;
}

bool mmu_t::mmio_fetch(reg_t paddr, size_t len, uint8_t* bytes)
{
  if (!mmio_ok(paddr, FETCH))
    return false;

  return sim->mmio_fetch(paddr, len, bytes);
}

bool mmu_t::mmio_load(reg_t paddr, size_t len, uint8_t* bytes)
{
  return mmio(paddr, len, bytes, LOAD);
}

bool mmu_t::mmio_store(reg_t paddr, size_t len, const uint8_t* bytes)
{
  return mmio(paddr, len, const_cast<uint8_t*>(bytes), STORE);
}

bool mmu_t::mmio(reg_t paddr, size_t len, uint8_t* bytes, access_type type)
{
  bool power_of_2 = (len & (len - 1)) == 0;
  bool naturally_aligned = (paddr & (len - 1)) == 0;

  if (power_of_2 && naturally_aligned) {
    if (!mmio_ok(paddr, type))
      return false;

    if (type == STORE)
      return sim->mmio_store(paddr, len, bytes);
    else
      return sim->mmio_load(paddr, len, bytes);
  }

  for (size_t i = 0; i < len; i++) {
    if (!mmio(paddr + i, 1, bytes + i, type))
      return false;
  }

  return true;
}

void mmu_t::check_triggers(triggers::operation_t operation, reg_t address, bool virt, reg_t tval, std::optional<reg_t> data)
{
  if (matched_trigger || !proc)
    return;

  auto match = proc->TM.detect_memory_access_match(operation, address, data);

  if (match.has_value())
    switch (match->timing) {
      case triggers::TIMING_BEFORE:
        throw triggers::matched_t(operation, tval, match->action, virt);

      case triggers::TIMING_AFTER:
        // We want to take this exception on the next instruction.  We check
        // whether to do so in the I$ refill slow path, which we can force by
        // flushing the TLB.
        flush_tlb();
        matched_trigger = triggers::matched_t(operation, tval, match->action, virt);
    }
}

inline void mmu_t::perform_intrapage_load(reg_t vaddr, uintptr_t host_addr, reg_t paddr, reg_t len, uint8_t* bytes, xlate_flags_t xlate_flags)
{
  if (host_addr) {
    memcpy(bytes, (char*)host_addr, len);
  } else if (!mmio_load(paddr, len, bytes)) {
    auto access_info = generate_access_info(vaddr, LOAD, xlate_flags);
    if (access_info.flags.ss_access)
      throw trap_store_access_fault(access_info.effective_virt, access_info.transformed_vaddr, 0, 0);
    else
      throw trap_load_access_fault(access_info.effective_virt, access_info.transformed_vaddr, 0, 0);
  }

  if (tracer.interested_in_range(paddr, paddr + len, LOAD))
    tracer.trace(paddr, len, LOAD);
}

void mmu_t::load_slow_path_intrapage(reg_t len, uint8_t* bytes, mem_access_info_t access_info)
{
  reg_t vaddr = access_info.vaddr;
  auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_load, vaddr, TLB_FLAGS);
  if (!tlb_hit || access_info.flags.is_special_access()) {
    paddr = translate(access_info, len);
    host_addr = (uintptr_t)sim->addr_to_mem(paddr);

    if (!access_info.flags.is_special_access())
      refill_tlb(vaddr, paddr, (char*)host_addr, LOAD);

    if (access_info.flags.lr && !sim->reservable(paddr)) {
      throw trap_load_access_fault(access_info.effective_virt, access_info.transformed_vaddr, 0, 0);
    }
  }

  perform_intrapage_load(vaddr, host_addr, paddr, len, bytes, access_info.flags);

  if (access_info.flags.lr) {
    load_reservation_address = paddr;
  }
}

void mmu_t::load_slow_path(reg_t original_addr, reg_t len, uint8_t* bytes, xlate_flags_t xlate_flags)
{
  if (likely(!xlate_flags.is_special_access())) {
    // Fast path for simple cases
    auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_load, original_addr, TLB_FLAGS & ~TLB_CHECK_TRIGGERS);
    bool intrapage = (original_addr % PGSIZE) + len <= PGSIZE;
    bool aligned = (original_addr & (len - 1)) == 0;

    if (likely(tlb_hit && (aligned || (intrapage && is_misaligned_enabled())))) {
      return perform_intrapage_load(original_addr, host_addr, paddr, len, bytes, xlate_flags);
    }
  }

  auto access_info = generate_access_info(original_addr, LOAD, xlate_flags);
  reg_t transformed_addr = access_info.transformed_vaddr;
  check_triggers(triggers::OPERATION_LOAD, transformed_addr, access_info.effective_virt);

  if ((transformed_addr & (len - 1)) == 0) {
    load_slow_path_intrapage(len, bytes, access_info);
  } else {
    bool gva = access_info.effective_virt;
    if (!is_misaligned_enabled())
      throw trap_load_address_misaligned(gva, transformed_addr, 0, 0);

    if (access_info.flags.lr)
      throw trap_load_access_fault(gva, transformed_addr, 0, 0);

    reg_t len_page0 = std::min(len, PGSIZE - transformed_addr % PGSIZE);
    load_slow_path_intrapage(len_page0, bytes, access_info);
    if (len_page0 != len) {
      auto tail_access_info = generate_access_info(original_addr + len_page0, LOAD, xlate_flags);
      load_slow_path_intrapage(len - len_page0, bytes + len_page0, tail_access_info);
    }
  }

  while (len > sizeof(reg_t)) {
    check_triggers(triggers::OPERATION_LOAD, transformed_addr, access_info.effective_virt, reg_from_bytes(sizeof(reg_t), bytes));
    len -= sizeof(reg_t);
    bytes += sizeof(reg_t);
  }
  check_triggers(triggers::OPERATION_LOAD, transformed_addr, access_info.effective_virt, reg_from_bytes(len, bytes));

  if (proc && unlikely(proc->get_log_commits_enabled()))
    proc->state.log_mem_read.push_back(std::make_tuple(original_addr, 0, len));
}

inline void mmu_t::perform_intrapage_store(reg_t vaddr, uintptr_t host_addr, reg_t paddr, reg_t len, const uint8_t* bytes, xlate_flags_t xlate_flags)
{
  if (host_addr) {
     memcpy((char*)host_addr, bytes, len);
  } else if (!mmio_store(paddr, len, bytes)) {
    auto access_info = generate_access_info(vaddr, STORE, xlate_flags);
    throw trap_store_access_fault(access_info.effective_virt, access_info.transformed_vaddr, 0, 0);
  }

  if (tracer.interested_in_range(paddr, paddr + len, STORE))
    tracer.trace(paddr, len, STORE);
}

void mmu_t::store_slow_path_intrapage(reg_t len, const uint8_t* bytes, mem_access_info_t access_info, bool actually_store)
{
  reg_t vaddr = access_info.vaddr;
  auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_store, vaddr, TLB_FLAGS);
  if (!tlb_hit || access_info.flags.is_special_access()) {
    paddr = translate(access_info, len);
    host_addr = (uintptr_t)sim->addr_to_mem(paddr);

    if (!access_info.flags.is_special_access())
      refill_tlb(vaddr, paddr, (char*)host_addr, STORE);
  }

  if (actually_store)
    perform_intrapage_store(vaddr, host_addr, paddr, len, bytes, access_info.flags);
}

void mmu_t::store_slow_path(reg_t original_addr, reg_t len, const uint8_t* bytes, xlate_flags_t xlate_flags, bool actually_store, bool UNUSED require_alignment)
{
  if (likely(!xlate_flags.is_special_access())) {
    // Fast path for simple cases
    auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_store, original_addr, TLB_FLAGS & ~TLB_CHECK_TRIGGERS);
    bool intrapage = (original_addr % PGSIZE) + len <= PGSIZE;
    bool aligned = (original_addr & (len - 1)) == 0;

    if (likely(tlb_hit && (aligned || (intrapage && is_misaligned_enabled())))) {
      if (actually_store)
        perform_intrapage_store(original_addr, host_addr, paddr, len, bytes, xlate_flags);
      return;
    }
  }

  auto access_info = generate_access_info(original_addr, STORE, xlate_flags);
  reg_t transformed_addr = access_info.transformed_vaddr;
  if (actually_store) {
    reg_t trig_len = len;
    const uint8_t* trig_bytes = bytes;
    while (trig_len > sizeof(reg_t)) {
      check_triggers(triggers::OPERATION_STORE, transformed_addr, access_info.effective_virt, reg_from_bytes(sizeof(reg_t), trig_bytes));
      trig_len -= sizeof(reg_t);
      trig_bytes += sizeof(reg_t);
    }
    check_triggers(triggers::OPERATION_STORE, transformed_addr, access_info.effective_virt, reg_from_bytes(trig_len, trig_bytes));
  }

  if (transformed_addr & (len - 1)) {
    bool gva = access_info.effective_virt;
    if (!is_misaligned_enabled())
      throw trap_store_address_misaligned(gva, transformed_addr, 0, 0);

    if (require_alignment)
      throw trap_store_access_fault(gva, transformed_addr, 0, 0);

    reg_t len_page0 = std::min(len, PGSIZE - transformed_addr % PGSIZE);
    store_slow_path_intrapage(len_page0, bytes, access_info, actually_store);
    if (len_page0 != len) {
      auto tail_access_info = generate_access_info(original_addr + len_page0, STORE, xlate_flags);
      store_slow_path_intrapage(len - len_page0, bytes + len_page0, tail_access_info, actually_store);
    }
  } else {
    store_slow_path_intrapage(len, bytes, access_info, actually_store);
  }

  if (actually_store && proc && unlikely(proc->get_log_commits_enabled())) {
    // amocas.q sends len == 16, reg_from_bytes only supports up to 8
    // bytes per conversion.  Make multiple entries in the log
    reg_t offset = 0;
    const auto reg_size = sizeof(reg_t);
    while (unlikely(len > reg_size)) {
      proc->state.log_mem_write.push_back(std::make_tuple(original_addr + offset, reg_from_bytes(reg_size, bytes + offset), reg_size));
      offset += reg_size;
      len -= reg_size;
    }
    proc->state.log_mem_write.push_back(std::make_tuple(original_addr + offset, reg_from_bytes(len, bytes + offset), len));
  }
}

tlb_entry_t mmu_t::refill_tlb(reg_t vaddr, reg_t paddr, char* host_addr, access_type type)
{
  reg_t idx = (vaddr >> PGSHIFT) % TLB_ENTRIES;
  reg_t expected_tag = vaddr >> PGSHIFT;
  reg_t base_paddr = paddr & ~reg_t(PGSIZE - 1);

  tlb_entry_t entry = {uintptr_t(host_addr) - (vaddr % PGSIZE), paddr - (vaddr % PGSIZE)};

  if (in_mprv()
      || !pmp_homogeneous(base_paddr, PGSIZE)
      || (proc && proc->get_log_commits_enabled()))
    return entry;

  auto trace_flag = tracer.interested_in_range(base_paddr, base_paddr + PGSIZE, type) ? TLB_CHECK_TRACER : 0;
  auto mmio_flag = host_addr ? 0 : TLB_MMIO;

  switch (type) {
    case FETCH:
      tlb_insn[idx].data = entry;
      tlb_insn[idx].tag = expected_tag | (check_triggers_fetch ? TLB_CHECK_TRIGGERS : 0) | trace_flag | mmio_flag;
      break;
    case LOAD:
      tlb_load[idx].data = entry;
      tlb_load[idx].tag = expected_tag | (check_triggers_load ? TLB_CHECK_TRIGGERS : 0) | trace_flag | mmio_flag;
      break;
    case STORE:
      tlb_store[idx].data = entry;
      tlb_store[idx].tag = expected_tag | (check_triggers_store ? TLB_CHECK_TRIGGERS : 0) | trace_flag | mmio_flag;
      break;
    default:
      abort();
  }

  return entry;
}

bool mmu_t::pmp_ok(reg_t addr, reg_t len, access_type type, reg_t mode, bool hlvx)
{
  if (!proc || proc->n_pmp == 0)
    return true;

  for (size_t i = 0; i < proc->n_pmp; i++) {
    // Check each 4-byte sector of the access
    bool any_match = false;
    bool all_match = true;
    for (reg_t offset = 0; offset < len; offset += 1 << PMP_SHIFT) {
      reg_t cur_addr = addr + offset;
      bool match = proc->state.pmpaddr[i]->match4(cur_addr);
      any_match |= match;
      all_match &= match;
    }

    if (any_match) {
      // If the PMP matches only a strict subset of the access, fail it
      if (!all_match)
        return false;

      return proc->state.pmpaddr[i]->access_ok(type, mode, hlvx);
    }
  }

  // in case matching region is not found
  const bool mseccfg_mml = proc->state.mseccfg->get_mml();
  const bool mseccfg_mmwp = proc->state.mseccfg->get_mmwp();
  return ((mode == PRV_M) && !mseccfg_mmwp
          && (!mseccfg_mml || ((type == LOAD) || (type == STORE))));
}

reg_t mmu_t::pmp_homogeneous(reg_t addr, reg_t len)
{
  if ((addr | len) & (len - 1))
    abort();

  if (!proc)
    return true;

  for (size_t i = 0; i < proc->n_pmp; i++)
    if (proc->state.pmpaddr[i]->subset_match(addr, len))
      return false;

  return true;
}

reg_t mmu_t::s2xlate(reg_t gva, reg_t gpa, access_type type, access_type trap_type, bool virt, bool hlvx, bool is_for_vs_pt_addr)
{
  if (!virt)
    return gpa;

  vm_info vm = decode_vm_info(proc->get_const_xlen(), true, 0, proc->get_state()->hgatp->read());
  if (vm.levels == 0)
    return gpa;

  int maxgpabits = vm.levels * vm.idxbits + vm.widenbits + PGSHIFT;
  reg_t maxgpa = (1ULL << maxgpabits) - 1;

  bool mxr = !is_for_vs_pt_addr && (proc->state.sstatus->readvirt(false) & MSTATUS_MXR);
  // tinst is set to 0x3000/0x3020 - for RV64 read/write respectively for
  // VS-stage address translation (for spike HSXLEN == VSXLEN always) else
  // tinst is set to 0x2000/0x2020 - for RV32 read/write respectively for
  // VS-stage address translation else set to 0
  int tinst = 0;
  tinst |= (is_for_vs_pt_addr == true) ? 0x2000 : 0;
  tinst |= ((proc->get_const_xlen() == 64) && (is_for_vs_pt_addr == true)) ? 0x1000 : 0;
  tinst |= ((type == STORE) && (is_for_vs_pt_addr == true)) ? 0x0020 : 0;

  reg_t base = vm.ptbase;
  if ((gpa & ~maxgpa) == 0) {
    for (int i = vm.levels - 1; i >= 0; i--) {
      int ptshift = i * vm.idxbits;
      int idxbits = (i == (vm.levels - 1)) ? vm.idxbits + vm.widenbits : vm.idxbits;
      reg_t idx = (gpa >> (PGSHIFT + ptshift)) & ((reg_t(1) << idxbits) - 1);

      // check that physical address of PTE is legal
      auto pte_paddr = base + idx * vm.ptesize;
      reg_t pte = pte_load(pte_paddr, gva, virt, trap_type, vm.ptesize);
      reg_t ppn = (pte & ~reg_t(PTE_ATTR)) >> PTE_PPN_SHIFT;
      bool pbmte = proc->get_state()->menvcfg->read() & MENVCFG_PBMTE;
      bool hade = proc->get_state()->menvcfg->read() & MENVCFG_ADUE;
      int napot_bits = ((pte & PTE_N) ? (ctz(ppn) + 1) : 0);

      if (pte & PTE_RSVD) {
        break;
      } else if (!proc->extension_enabled(EXT_SVNAPOT) && (pte & PTE_N)) {
        break;
      } else if (!pbmte && (pte & PTE_PBMT)) {
        break;
      } else if ((pte & PTE_PBMT) == PTE_PBMT) {
        break;
      } else if (PTE_TABLE(pte)) { // next level of page table
        if (pte & (PTE_D | PTE_A | PTE_U | PTE_N | PTE_PBMT))
          break;
        base = ppn << PGSHIFT;
      } else if (!(pte & PTE_V) || (!(pte & PTE_R) && (pte & PTE_W))) {
        break;
      } else if (((pte & PTE_N) && (ppn == 0 || i != 0)) || (napot_bits != 0 && napot_bits != 4)) {
        break;
      } else if (!(pte & PTE_U)) {
        break;
      } else if (type == FETCH || hlvx ? !(pte & PTE_X) :
                 type == LOAD          ? !(pte & PTE_R) && !(mxr && (pte & PTE_X)) :
                                         !((pte & PTE_R) && (pte & PTE_W))) {
        break;
      } else if ((ppn & ((reg_t(1) << ptshift) - 1)) != 0) {
        break;
      } else {
        reg_t ad = PTE_A | ((type == STORE) * PTE_D);

        if ((pte & ad) != ad) {
          if (hade) {
            // set accessed and possibly dirty bits
            pte_store(pte_paddr, pte | ad, gva, virt, type, vm.ptesize);
          } else {
            // take exception if access or possibly dirty bit is not set.
            break;
          }
        }

        reg_t vpn = gpa >> PGSHIFT;
        reg_t page_mask = (reg_t(1) << PGSHIFT) - 1;

        reg_t page_base = ((ppn & ~((reg_t(1) << napot_bits) - 1))
                          | (vpn & ((reg_t(1) << napot_bits) - 1))
                          | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;
        return page_base | (gpa & page_mask);
      }
    }
  }

  switch (trap_type) {
    case FETCH: throw trap_instruction_guest_page_fault(gva, gpa >> 2, tinst);
    case LOAD: throw trap_load_guest_page_fault(gva, gpa >> 2, tinst);
    case STORE: throw trap_store_guest_page_fault(gva, gpa >> 2, tinst);
    default: abort();
  }
}

reg_t mmu_t::walk(mem_access_info_t access_info)
{
  access_type type = access_info.type;
  reg_t addr = access_info.transformed_vaddr;
  bool virt = access_info.effective_virt;
  bool hlvx = access_info.flags.hlvx;
  reg_t mode = access_info.effective_priv;
  reg_t page_mask = (reg_t(1) << PGSHIFT) - 1;
  reg_t satp = proc->get_state()->satp->readvirt(virt);
  vm_info vm = decode_vm_info(proc->get_const_xlen(), false, mode, satp);

  bool ss_access = access_info.flags.ss_access;

  if (ss_access) {
    if (vm.levels == 0)
      throw trap_store_access_fault(virt, addr, 0, 0);
    type = STORE;
  }

  if (vm.levels == 0)
    return s2xlate(addr, addr & ((reg_t(2) << (proc->xlen-1))-1), type, type, virt, hlvx, false) & ~page_mask; // zero-extend from xlen

  bool s_mode = mode == PRV_S;
  bool sum = proc->state.sstatus->readvirt(virt) & MSTATUS_SUM;
  bool mxr = (proc->state.sstatus->readvirt(false) | proc->state.sstatus->readvirt(virt)) & MSTATUS_MXR;

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
    auto pte_paddr = s2xlate(addr, base + idx * vm.ptesize, LOAD, type, virt, false, true);
    reg_t pte = pte_load(pte_paddr, addr, virt, type, vm.ptesize);
    reg_t ppn = (pte & ~reg_t(PTE_ATTR)) >> PTE_PPN_SHIFT;
    bool pbmte = virt ? (proc->get_state()->henvcfg->read() & HENVCFG_PBMTE) : (proc->get_state()->menvcfg->read() & MENVCFG_PBMTE);
    bool hade = virt ? (proc->get_state()->henvcfg->read() & HENVCFG_ADUE) : (proc->get_state()->menvcfg->read() & MENVCFG_ADUE);
    bool sse = virt ? (proc->get_state()->henvcfg->read() & HENVCFG_SSE) : (proc->get_state()->menvcfg->read() & MENVCFG_SSE);
    bool ss_page = !(pte & PTE_R) && (pte & PTE_W) && !(pte & PTE_X);
    int napot_bits = ((pte & PTE_N) ? (ctz(ppn) + 1) : 0);

    if (pte & PTE_RSVD) {
      break;
    } else if (!proc->extension_enabled(EXT_SVNAPOT) && (pte & PTE_N)) {
      break;
    } else if (!pbmte && (pte & PTE_PBMT)) {
      break;
    } else if ((pte & PTE_PBMT) == PTE_PBMT) {
      break;
    } else if (PTE_TABLE(pte)) { // next level of page table
      if (pte & (PTE_D | PTE_A | PTE_U | PTE_N | PTE_PBMT))
        break;
      base = ppn << PGSHIFT;
    } else if ((pte & PTE_U) ? s_mode && (type == FETCH || !sum) : !s_mode) {
      break;
    } else if (!(pte & PTE_V) ||
              (!(pte & PTE_R) && (pte & PTE_W) && ((!sse && !(pte & PTE_X)) || (pte & PTE_X)))) {
      // invalid
      // not shadow stack access xwr=110 or xwr=010 page cause page fault
      // shadow stack access with PTE_X moved to following check
      break;
    } else if (((pte & PTE_N) && (ppn == 0 || i != 0)) || (napot_bits != 0 && napot_bits != 4)) {
      break;
    } else if ((ppn & ((reg_t(1) << ptshift) - 1)) != 0) {
      break;
    } else if (ss_page && ((type == STORE && !ss_access) || access_info.flags.clean_inval)) {
      // non-shadow-stack store or CBO with xwr = 010 causes access-fault
      throw trap_store_access_fault(virt, addr, 0, 0);
    } else if (ss_page && type == FETCH) {
      // fetch from shadow stack pages cause instruction access-fault
      throw trap_instruction_access_fault(virt, addr, 0, 0);
    } else if ((((pte & PTE_R) && (pte & PTE_W)) || (pte & PTE_X)) && ss_access) {
      // shadow stack access cause store access fault if xwr!=010 and xwr!=001
      throw trap_store_access_fault(virt, addr, 0, 0);
    } else if (type == FETCH || hlvx ? !(pte & PTE_X) :
               type == LOAD          ? !(sse && ss_page) && !(pte & PTE_R) && !(mxr && (pte & PTE_X)) :
                                       !(pte & PTE_W)) {
      break;
    } else {
      reg_t ad = PTE_A | ((type == STORE) * PTE_D);

      if ((pte & ad) != ad) {
        if (hade) {
          // Check for write permission to the first-stage PT in second-stage
          // PTE and set the D bit in the second-stage PTE if needed
          s2xlate(addr, base + idx * vm.ptesize, STORE, type, virt, false, true);
          // set accessed and possibly dirty bits.
          pte_store(pte_paddr, pte | ad, addr, virt, type, vm.ptesize);
        } else {
          // take exception if access or possibly dirty bit is not set.
          break;
        }
      }

      // for superpage or Svnapot NAPOT mappings, make a fake leaf PTE for the TLB's benefit.
      reg_t vpn = addr >> PGSHIFT;

      reg_t page_base = ((ppn & ~((reg_t(1) << napot_bits) - 1))
                        | (vpn & ((reg_t(1) << napot_bits) - 1))
                        | (vpn & ((reg_t(1) << ptshift) - 1))) << PGSHIFT;
      reg_t phys = page_base | (addr & page_mask);
      return s2xlate(addr, phys, type, type, virt, hlvx, false) & ~page_mask;
    }
  }

  switch (type) {
    case FETCH: throw trap_instruction_page_fault(virt, addr, 0, 0);
    case LOAD: throw trap_load_page_fault(virt, addr, 0, 0);
    case STORE: throw trap_store_page_fault(virt, addr, 0, 0);
    default: abort();
  }
}

void mmu_t::register_memtracer(memtracer_t* t)
{
  flush_tlb();
  tracer.hook(t);
}

reg_t mmu_t::get_pmlen(bool effective_virt, reg_t effective_priv, xlate_flags_t flags) const {
  if (!proc || proc->get_xlen() != 64 || flags.hlvx)
    return 0;

  reg_t pmm = 0;
  if (effective_priv == PRV_M)
    pmm = get_field(proc->state.mseccfg->read(), MSECCFG_PMM);
  else if ((proc->state.sstatus->readvirt(false) | proc->state.sstatus->readvirt(effective_virt)) & MSTATUS_MXR)
    pmm = 0;
  else if (!effective_virt && (effective_priv == PRV_S || (!proc->extension_enabled('S') && effective_priv == PRV_U)))
    pmm = get_field(proc->state.menvcfg->read(), MENVCFG_PMM);
  else if (effective_virt && effective_priv == PRV_S)
    pmm = get_field(proc->state.henvcfg->read(), HENVCFG_PMM);
  else if (proc->state.prv == PRV_U && flags.forced_virt)
    pmm = get_field(proc->state.hstatus->read(), HSTATUS_HUPMM);
  else if (effective_priv == PRV_U)
    pmm = get_field(proc->state.senvcfg->read(), SENVCFG_PMM);
  else
    assert(false);

  switch (pmm) {
    case 2: return 7;
    case 3: return 16;
  }
  return 0;
}

mem_access_info_t mmu_t::generate_access_info(reg_t addr, access_type type, xlate_flags_t xlate_flags) {
  if (!proc)
    return {addr, addr, 0, false, {}, type};
  bool virt = proc->state.v;
  reg_t mode = proc->state.prv;
  reg_t transformed_addr = addr;
  if (type != FETCH) {
    if (in_mprv()) {
      mode = get_field(proc->state.mstatus->read(), MSTATUS_MPP);
      if (get_field(proc->state.mstatus->read(), MSTATUS_MPV) && mode != PRV_M)
        virt = true;
    }
    if (xlate_flags.forced_virt) {
      virt = true;
      mode = get_field(proc->state.hstatus->read(), HSTATUS_SPVP);
    }
    auto xlen = proc->get_const_xlen();
    reg_t pmlen = get_pmlen(virt, mode, xlate_flags);
    reg_t satp = proc->state.satp->readvirt(virt);
    bool is_physical_addr = mode == PRV_M || get_field(satp, SATP64_MODE) == SATP_MODE_OFF;
    transformed_addr = is_physical_addr ? zext(addr, xlen - pmlen) : sext(addr, xlen - pmlen);
  }
  return {addr, transformed_addr, mode, virt, xlate_flags, type};
}
