// See LICENSE for license details.

#ifndef _RISCV_MMU_H
#define _RISCV_MMU_H

#include "decode.h"
#include "trap.h"
#include "common.h"
#include "simif.h"
#include "processor.h"
#include "memtracer.h"
#include "../fesvr/byteorder.h"
#include "triggers.h"
#include "cfg.h"
#include <stdlib.h>
#include <vector>

// virtual memory configuration
#define PGSHIFT 12
const reg_t PGSIZE = 1 << PGSHIFT;

// observability hooks for load, store and fetch
// intentionally empty not to cause runtime overhead
// can be redefined if needed 
#ifndef MMU_OBSERVE_FETCH
#define MMU_OBSERVE_FETCH(addr, insn, length)
#endif

#ifndef MMU_OBSERVE_LOAD
#define MMU_OBSERVE_LOAD(addr, data, length)
#endif

#ifndef MMU_OBSERVE_STORE
#define MMU_OBSERVE_STORE(addr, data, length)
#endif

struct insn_fetch_t
{
  insn_func_t func;
  insn_t insn;
};

struct icache_entry_t {
  reg_t tag;
  struct icache_entry_t* next;
  insn_fetch_t data;
};

struct tlb_entry_t {
  uintptr_t host_addr;
  reg_t target_addr;
};

struct dtlb_entry_t {
  tlb_entry_t data;
  reg_t tag;
};

struct xlate_flags_t {
  const bool forced_virt : 1 {false};
  const bool hlvx : 1 {false};
  const bool lr : 1 {false};
  const bool ss_access : 1 {false};
  const bool clean_inval : 1 {false};

  bool is_special_access() const {
    return forced_virt || hlvx || lr || ss_access || clean_inval;
  }
};

struct mem_access_info_t {
  const reg_t vaddr;
  const reg_t transformed_vaddr;
  const reg_t effective_priv;
  const bool effective_virt;
  const xlate_flags_t flags;
  const access_type type;
};

void throw_access_exception(bool virt, reg_t addr, access_type type);

// this class implements a processor's port into the virtual memory system.
// an MMU and instruction cache are maintained for simulator performance.
class mmu_t
{
private:
  reg_t get_pmlen(bool effective_virt, reg_t effective_priv, xlate_flags_t flags) const;
  mem_access_info_t generate_access_info(reg_t addr, access_type type, xlate_flags_t xlate_flags);

public:
  mmu_t(simif_t* sim, endianness_t endianness, processor_t* proc, reg_t cache_blocksz);
  ~mmu_t();

  template<typename T>
  T ALWAYS_INLINE load(reg_t addr, xlate_flags_t xlate_flags = {}) {
    target_endian<T> res;
    bool aligned = (addr & (sizeof(T) - 1)) == 0;
    auto [tlb_hit, host_addr, _] = access_tlb(tlb_load, addr);

    if (likely(!xlate_flags.is_special_access() && aligned && tlb_hit)) {
      res = *(target_endian<T>*)host_addr;
    } else {
      load_slow_path(addr, sizeof(T), (uint8_t*)&res, xlate_flags);
    }

    MMU_OBSERVE_LOAD(addr,from_target(res),sizeof(T));

    return from_target(res);
  }

  template<typename T>
  T load_reserved(reg_t addr) {
    return load<T>(addr, {.lr = true});
  }

  template<typename T>
  T guest_load(reg_t addr) {
    return load<T>(addr, {.forced_virt = true});
  }

  template<typename T>
  T guest_load_x(reg_t addr) {
    return load<T>(addr, {.forced_virt=true, .hlvx=true});
  }

  // shadow stack load
  template<typename T>
  T ss_load(reg_t addr) {
    if ((addr & (sizeof(T) - 1)) != 0)
      throw trap_store_access_fault((proc) ? proc->state.v : false, addr, 0, 0);
    return load<T>(addr, {.forced_virt=false, .hlvx=false, .lr=false, .ss_access=true});
  }

  template<typename T>
  void ALWAYS_INLINE store(reg_t addr, T val, xlate_flags_t xlate_flags = {}) {
    MMU_OBSERVE_STORE(addr, val, sizeof(T));
    bool aligned = (addr & (sizeof(T) - 1)) == 0;
    auto [tlb_hit, host_addr, _] = access_tlb(tlb_store, addr);

    if (!xlate_flags.is_special_access() && likely(aligned && tlb_hit)) {
      *(target_endian<T>*)host_addr = to_target(val);
    } else {
      target_endian<T> target_val = to_target(val);
      store_slow_path(addr, sizeof(T), (const uint8_t*)&target_val, xlate_flags, true, false);
    }
  }

  template<typename T>
  void guest_store(reg_t addr, T val) {
    store(addr, val, {.forced_virt=true});
  }

  // shadow stack store
  template<typename T>
  void ss_store(reg_t addr, T val) {
    if ((addr & (sizeof(T) - 1)) != 0)
      throw trap_store_access_fault((proc) ? proc->state.v : false, addr, 0, 0);
    store<T>(addr, val, {.forced_virt=false, .hlvx=false, .lr=false, .ss_access=true});
  }

  // AMO/Zicbom faults should be reported as store faults
  #define convert_load_traps_to_store_traps(BODY) \
    try { \
      BODY \
    } catch (trap_load_address_misaligned& t) { \
      /* Misaligned fault will not be triggered by Zicbom */ \
      throw trap_store_address_misaligned(t.has_gva(), t.get_tval(), t.get_tval2(), t.get_tinst()); \
    } catch (trap_load_page_fault& t) { \
      throw trap_store_page_fault(t.has_gva(), t.get_tval(), t.get_tval2(), t.get_tinst()); \
    } catch (trap_load_access_fault& t) { \
      throw trap_store_access_fault(t.has_gva(), t.get_tval(), t.get_tval2(), t.get_tinst()); \
    } catch (trap_load_guest_page_fault& t) { \
      throw trap_store_guest_page_fault(t.get_tval(), t.get_tval2(), t.get_tinst()); \
    }

  // template for functions that perform an atomic memory operation
  template<typename T, typename op>
  T amo(reg_t addr, op f) {
    convert_load_traps_to_store_traps({
      store_slow_path(addr, sizeof(T), nullptr, {}, false, true);
      auto lhs = load<T>(addr);
      store<T>(addr, f(lhs));
      return lhs;
    })
  }

  // for shadow stack amoswap
  template<typename T>
  T ssamoswap(reg_t addr, reg_t value) {
      bool forced_virt = false;
      bool hlvx = false;
      bool lr = false;
      bool ss_access = true;
      store_slow_path(addr, sizeof(T), nullptr, {forced_virt, hlvx, lr, ss_access}, false, true);
      auto data = load<T>(addr, {forced_virt, hlvx, lr, ss_access});
      store<T>(addr, value, {forced_virt, hlvx, lr, ss_access});
      return data;
  }

  template<typename T>
  T amo_compare_and_swap(reg_t addr, T comp, T swap) {
    convert_load_traps_to_store_traps({
      store_slow_path(addr, sizeof(T), nullptr, {}, false, true);
      auto lhs = load<T>(addr);
      if (lhs == comp)
        store<T>(addr, swap);
      return lhs;
    })
  }

  void store_float128(reg_t addr, float128_t val)
  {
    if (unlikely(addr & (sizeof(float128_t)-1)) && !is_misaligned_enabled()) {
      throw trap_store_address_misaligned((proc) ? proc->state.v : false, addr, 0, 0);
    }

    store<uint64_t>(addr, val.v[0]);
    store<uint64_t>(addr + 8, val.v[1]);
  }

  float128_t load_float128(reg_t addr)
  {
    if (unlikely(addr & (sizeof(float128_t)-1)) && !is_misaligned_enabled()) {
      throw trap_load_address_misaligned((proc) ? proc->state.v : false, addr, 0, 0);
    }

    float128_t res;
    res.v[0] = load<uint64_t>(addr);
    res.v[1] = load<uint64_t>(addr + 8);
    return res;
  }

  void cbo_zero(reg_t addr) {
    auto access_info = generate_access_info(addr, STORE, {});
    reg_t transformed_addr = access_info.transformed_vaddr;

    auto base = transformed_addr & ~(blocksz - 1);
    for (size_t offset = 0; offset < blocksz; offset += 1) {
      check_triggers(triggers::OPERATION_STORE, base + offset, false, transformed_addr, std::nullopt);
      store<uint8_t>(base + offset, 0);
    }
  }

  void clean_inval(reg_t addr, bool clean, bool inval) {
    auto access_info = generate_access_info(addr, LOAD, {.clean_inval = true});
    reg_t transformed_addr = access_info.transformed_vaddr;

    auto base = transformed_addr & ~(blocksz - 1);
    for (size_t offset = 0; offset < blocksz; offset += 1)
      check_triggers(triggers::OPERATION_STORE, base + offset, false, transformed_addr, std::nullopt);
    convert_load_traps_to_store_traps({
      const reg_t paddr = translate(access_info, 1);
      if (sim->reservable(paddr)) {
        if (tracer.interested_in_range(paddr, paddr + PGSIZE, LOAD))
          tracer.clean_invalidate(paddr, blocksz, clean, inval);
      } else {
        throw trap_store_access_fault((proc) ? proc->state.v : false, transformed_addr, 0, 0);
      }
    })
  }

  inline void yield_load_reservation()
  {
    load_reservation_address = (reg_t)-1;
  }

  inline bool check_load_reservation(reg_t vaddr, size_t size)
  {
    if (vaddr & (size-1)) {
      // Raise either access fault or misaligned exception
      store_slow_path(vaddr, size, nullptr, {}, false, true);
    }

    reg_t paddr = translate(generate_access_info(vaddr, STORE, {}), 1);
    if (sim->reservable(paddr))
      return load_reservation_address == paddr;
    else
      throw trap_store_access_fault((proc) ? proc->state.v : false, vaddr, 0, 0);
  }

  template<typename T>
  bool store_conditional(reg_t addr, T val)
  {
    bool have_reservation = check_load_reservation(addr, sizeof(T));

    if (have_reservation)
      store(addr, val);

    yield_load_reservation();

    return have_reservation;
  }

  static const reg_t ICACHE_ENTRIES = 1024;

  inline size_t icache_index(reg_t addr)
  {
    return (addr / PC_ALIGN) % ICACHE_ENTRIES;
  }

  template<typename T>
  T ALWAYS_INLINE fetch_jump_table(reg_t addr) {
    T res = 0;
    for (size_t i = 0; i < sizeof(T) / sizeof(insn_parcel_t); i++)
      res |= (T)fetch_insn_parcel(addr + i * sizeof(insn_parcel_t)) << (i * sizeof(insn_parcel_t) * 8);

    // table accesses use data endianness, not instruction (little) endianness
    return target_big_endian ? to_be(res) : res;
  }

  inline icache_entry_t* refill_icache(reg_t addr, icache_entry_t* entry)
  {
    insn_bits_t insn = fetch_insn_parcel(addr);

    int length = insn_length(insn);

    if (likely(length == 4)) {
      insn |= (insn_bits_t)fetch_insn_parcel(addr + 2) << 16;
    } else if (length == 2) {
      // entire instruction already fetched
    } else if (length == 6) {
      insn |= (insn_bits_t)fetch_insn_parcel(addr + 2) << 16;
      insn |= (insn_bits_t)fetch_insn_parcel(addr + 4) << 32;
    } else {
      static_assert(sizeof(insn_bits_t) == 8, "insn_bits_t must be uint64_t");
      insn |= (insn_bits_t)fetch_insn_parcel(addr + 2) << 16;
      insn |= (insn_bits_t)fetch_insn_parcel(addr + 4) << 32;
      insn |= (insn_bits_t)fetch_insn_parcel(addr + 6) << 48;
    }

    insn_fetch_t fetch = {proc->decode_insn(insn), insn};
    entry->tag = addr;
    entry->next = &icache[icache_index(addr + length)];
    entry->data = fetch;

    auto [check_tracer, _, paddr] = access_tlb(tlb_insn, addr, TLB_FLAGS, TLB_CHECK_TRACER);
    if (unlikely(check_tracer)) {
      if (tracer.interested_in_range(paddr, paddr + 1, FETCH)) {
        entry->tag = -1;
        tracer.trace(paddr, paddr + length, FETCH);
      }
    }
    MMU_OBSERVE_FETCH(addr, insn, length);
    return entry;
  }

  inline icache_entry_t* access_icache(reg_t addr)
  {
    icache_entry_t* entry = &icache[icache_index(addr)];
    if (likely(entry->tag == addr)){
      MMU_OBSERVE_FETCH(addr, entry->data.insn, insn_length(entry->data.insn.bits()));
      return entry;
    }
    return refill_icache(addr, entry);
  }

  inline insn_fetch_t load_insn(reg_t addr)
  {
    icache_entry_t entry;
    return refill_icache(addr, &entry)->data;
  }

  std::tuple<bool, uintptr_t, reg_t> ALWAYS_INLINE access_tlb(const dtlb_entry_t* tlb, reg_t vaddr, reg_t allowed_flags = 0, reg_t required_flags = 0)
  {
    auto vpn = vaddr / PGSIZE, pgoff = vaddr % PGSIZE;
    auto& entry = tlb[vpn % TLB_ENTRIES];
    auto hit = likely((entry.tag & (~allowed_flags | required_flags)) == (vpn | required_flags));
    bool mmio = allowed_flags & TLB_MMIO & entry.tag;
    auto host_addr = mmio ? 0 : entry.data.host_addr + pgoff;
    auto paddr = entry.data.target_addr + pgoff;
    return std::make_tuple(hit, host_addr, paddr);
  }

  void flush_tlb();
  void flush_icache();

  void register_memtracer(memtracer_t*);

  int is_misaligned_enabled()
  {
    return proc && proc->get_cfg().misaligned;
  }

  bool is_target_big_endian()
  {
    return target_big_endian;
  }

  template<typename T> inline T from_target(target_endian<T> n) const
  {
    return target_big_endian? n.from_be() : n.from_le();
  }

  template<typename T> inline target_endian<T> to_target(T n) const
  {
    return target_big_endian? target_endian<T>::to_be(n) : target_endian<T>::to_le(n);
  }

private:
  simif_t* sim;
  processor_t* proc;
  memtracer_list_t tracer;
  reg_t load_reservation_address;
  reg_t blocksz;

  // implement an instruction cache for simulator performance
  icache_entry_t icache[ICACHE_ENTRIES];

  // implement a TLB for simulator performance
  static const reg_t TLB_ENTRIES = 256;
  // If a TLB tag has TLB_CHECK_TRIGGERS set, then the MMU must check for a
  // trigger match before completing an access.
  static const reg_t TLB_CHECK_TRIGGERS = reg_t(1) << 63;
  static const reg_t TLB_CHECK_TRACER = reg_t(1) << 62;
  static const reg_t TLB_MMIO = reg_t(1) << 61;
  static const reg_t TLB_FLAGS = TLB_CHECK_TRIGGERS | TLB_CHECK_TRACER | TLB_MMIO;
  dtlb_entry_t tlb_load[TLB_ENTRIES];
  dtlb_entry_t tlb_store[TLB_ENTRIES];
  dtlb_entry_t tlb_insn[TLB_ENTRIES];

  // finish translation on a TLB miss and update the TLB
  tlb_entry_t refill_tlb(reg_t vaddr, reg_t paddr, char* host_addr, access_type type);
  const char* fill_from_mmio(reg_t vaddr, reg_t paddr);

  // perform a stage2 translation for a given guest address
  reg_t s2xlate(reg_t gva, reg_t gpa, access_type type, access_type trap_type, bool virt, bool hlvx, bool is_for_vs_pt_addr);

  // perform a page table walk for a given VA; set referenced/dirty bits
  reg_t walk(mem_access_info_t access_info);

  // handle uncommon cases: TLB misses, page faults, MMIO
  typedef uint16_t insn_parcel_t;
  insn_parcel_t fetch_slow_path(reg_t addr);
  insn_parcel_t perform_intrapage_fetch(reg_t vaddr, uintptr_t host_addr, reg_t paddr);
  void load_slow_path(reg_t original_addr, reg_t len, uint8_t* bytes, xlate_flags_t xlate_flags);
  void load_slow_path_intrapage(reg_t len, uint8_t* bytes, mem_access_info_t access_info);
  void perform_intrapage_load(reg_t vaddr, uintptr_t host_addr, reg_t paddr, reg_t len, uint8_t* bytes, xlate_flags_t xlate_flags);
  void store_slow_path(reg_t original_addr, reg_t len, const uint8_t* bytes, xlate_flags_t xlate_flags, bool actually_store, bool require_alignment);
  void store_slow_path_intrapage(reg_t len, const uint8_t* bytes, mem_access_info_t access_info, bool actually_store);
  void perform_intrapage_store(reg_t vaddr, uintptr_t host_addr, reg_t paddr, reg_t len, const uint8_t* bytes, xlate_flags_t xlate_flags);
  bool mmio_fetch(reg_t paddr, size_t len, uint8_t* bytes);
  bool mmio_load(reg_t paddr, size_t len, uint8_t* bytes);
  bool mmio_store(reg_t paddr, size_t len, const uint8_t* bytes);
  bool mmio(reg_t paddr, size_t len, uint8_t* bytes, access_type type);
  bool mmio_ok(reg_t paddr, access_type type);
  void check_triggers(triggers::operation_t operation, reg_t address, bool virt, std::optional<reg_t> data = std::nullopt) {
    check_triggers(operation, address, virt, address, data);
  }
  void check_triggers(triggers::operation_t operation, reg_t address, bool virt, reg_t tval, std::optional<reg_t> data);
  reg_t translate(mem_access_info_t access_info, reg_t len);

  reg_t pte_load(reg_t pte_paddr, reg_t addr, bool virt, access_type trap_type, size_t ptesize) {
    if (ptesize == 4)
      return pte_load<uint32_t>(pte_paddr, addr, virt, trap_type);
    else
      return pte_load<uint64_t>(pte_paddr, addr, virt, trap_type);
  }

  void pte_store(reg_t pte_paddr, reg_t new_pte, reg_t addr, bool virt, access_type trap_type, size_t ptesize) {
    if (ptesize == 4)
      return pte_store<uint32_t>(pte_paddr, new_pte, addr, virt, trap_type);
    else
      return pte_store<uint64_t>(pte_paddr, new_pte, addr, virt, trap_type);
  }

  template<typename T> inline reg_t pte_load(reg_t pte_paddr, reg_t addr, bool virt, access_type trap_type)
  {
    const size_t ptesize = sizeof(T);

    if (!pmp_ok(pte_paddr, ptesize, LOAD, PRV_S, false))
      throw_access_exception(virt, addr, trap_type);

    void* host_pte_addr = sim->addr_to_mem(pte_paddr);
    target_endian<T> target_pte;
    if (host_pte_addr) {
      memcpy(&target_pte, host_pte_addr, ptesize);
    } else if (!mmio_load(pte_paddr, ptesize, (uint8_t*)&target_pte)) {
      throw_access_exception(virt, addr, trap_type);
    }
    return from_target(target_pte);
  }

  template<typename T> inline void pte_store(reg_t pte_paddr, reg_t new_pte, reg_t addr, bool virt, access_type trap_type)
  {
    const size_t ptesize = sizeof(T);

    if (!pmp_ok(pte_paddr, ptesize, STORE, PRV_S, false))
      throw_access_exception(virt, addr, trap_type);

    void* host_pte_addr = sim->addr_to_mem(pte_paddr);
    target_endian<T> target_pte = to_target((T)new_pte);
    if (host_pte_addr) {
      memcpy(host_pte_addr, &target_pte, ptesize);
    } else if (!mmio_store(pte_paddr, ptesize, (uint8_t*)&target_pte)) {
      throw_access_exception(virt, addr, trap_type);
    }
  }

  inline insn_parcel_t fetch_insn_parcel(reg_t addr) {
    if (auto [tlb_hit, host_addr, paddr] = access_tlb(tlb_insn, addr); tlb_hit)
      return from_le(*(insn_parcel_t*)host_addr);

    return from_le(fetch_slow_path(addr));
  }

  inline bool in_mprv() const
  {
    return proc != nullptr
           && !(proc->state.mnstatus && !get_field(proc->state.mnstatus->read(), MNSTATUS_NMIE))
           && (!proc->state.debug_mode || get_field(proc->state.dcsr->read(), DCSR_MPRVEN))
           && get_field(proc->state.mstatus->read(), MSTATUS_MPRV);
  }

  reg_t pmp_homogeneous(reg_t addr, reg_t len);
  bool pmp_ok(reg_t addr, reg_t len, access_type type, reg_t mode, bool hlvx);

#ifdef RISCV_ENABLE_DUAL_ENDIAN
  bool target_big_endian;
#else
  static const bool target_big_endian = false;
#endif
  bool check_triggers_fetch;
  bool check_triggers_load;
  bool check_triggers_store;
  std::optional<triggers::matched_t> matched_trigger;

  friend class processor_t;
};

struct vm_info {
  int levels;
  int idxbits;
  int widenbits;
  int ptesize;
  reg_t ptbase;
};

inline vm_info decode_vm_info(int xlen, bool stage2, reg_t prv, reg_t satp)
{
  if (prv == PRV_M) {
    return {0, 0, 0, 0, 0};
  } else if (!stage2 && prv <= PRV_S && xlen == 32) {
    switch (get_field(satp, SATP32_MODE)) {
      case SATP_MODE_OFF: return {0, 0, 0, 0, 0};
      case SATP_MODE_SV32: return {2, 10, 0, 4, (satp & SATP32_PPN) << PGSHIFT};
      default: abort();
    }
  } else if (!stage2 && prv <= PRV_S && xlen == 64) {
    switch (get_field(satp, SATP64_MODE)) {
      case SATP_MODE_OFF: return {0, 0, 0, 0, 0};
      case SATP_MODE_SV39: return {3, 9, 0, 8, (satp & SATP64_PPN) << PGSHIFT};
      case SATP_MODE_SV48: return {4, 9, 0, 8, (satp & SATP64_PPN) << PGSHIFT};
      case SATP_MODE_SV57: return {5, 9, 0, 8, (satp & SATP64_PPN) << PGSHIFT};
      case SATP_MODE_SV64: return {6, 9, 0, 8, (satp & SATP64_PPN) << PGSHIFT};
      default: abort();
    }
  } else if (stage2 && xlen == 32) {
    switch (get_field(satp, HGATP32_MODE)) {
      case HGATP_MODE_OFF: return {0, 0, 0, 0, 0};
      case HGATP_MODE_SV32X4: return {2, 10, 2, 4, (satp & HGATP32_PPN) << PGSHIFT};
      default: abort();
    }
  } else if (stage2 && xlen == 64) {
    switch (get_field(satp, HGATP64_MODE)) {
      case HGATP_MODE_OFF: return {0, 0, 0, 0, 0};
      case HGATP_MODE_SV39X4: return {3, 9, 2, 8, (satp & HGATP64_PPN) << PGSHIFT};
      case HGATP_MODE_SV48X4: return {4, 9, 2, 8, (satp & HGATP64_PPN) << PGSHIFT};
      case HGATP_MODE_SV57X4: return {5, 9, 2, 8, (satp & HGATP64_PPN) << PGSHIFT};
      default: abort();
    }
  } else {
    abort();
  }
}

#endif
