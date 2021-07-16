// See LICENSE for license details.

#ifndef _RISCV_MMU_H
#define _RISCV_MMU_H

#include "decode.h"
#include "trap.h"
#include "common.h"
#include "config.h"
#include "simif.h"
#include "processor.h"
#include "memtracer.h"
#include "byteorder.h"
#include <stdlib.h>
#include <vector>

// virtual memory configuration
#define PGSHIFT 12
const reg_t PGSIZE = 1 << PGSHIFT;
const reg_t PGMASK = ~(PGSIZE-1);
#define MAX_PADDR_BITS 56 // imposed by Sv39 / Sv48

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
  char* host_offset;
  reg_t target_offset;
};

class trigger_matched_t
{
  public:
    trigger_matched_t(int index,
        trigger_operation_t operation, reg_t address, reg_t data) :
      index(index), operation(operation), address(address), data(data) {}

    int index;
    trigger_operation_t operation;
    reg_t address;
    reg_t data;
};

// this class implements a processor's port into the virtual memory system.
// an MMU and instruction cache are maintained for simulator performance.
class mmu_t
{
private:
  std::map<reg_t, reg_t> alloc_cache;
  std::vector<std::pair<reg_t, reg_t >> addr_tbl;
public:
  mmu_t(simif_t* sim, processor_t* proc);
  ~mmu_t();

  inline reg_t misaligned_load(reg_t addr, size_t size)
  {
#ifdef RISCV_ENABLE_MISALIGNED
    reg_t res = 0;
    for (size_t i = 0; i < size; i++)
      res += (reg_t)load_uint8(addr + (target_big_endian? size-1-i : i)) << (i * 8);
    return res;
#else
    throw trap_load_address_misaligned(addr, 0, 0);
#endif
  }

  inline void misaligned_store(reg_t addr, reg_t data, size_t size)
  {
#ifdef RISCV_ENABLE_MISALIGNED
    for (size_t i = 0; i < size; i++)
      store_uint8(addr + (target_big_endian? size-1-i : i), data >> (i * 8));
#else
    throw trap_store_address_misaligned(addr, 0, 0);
#endif
  }

#ifndef RISCV_ENABLE_COMMITLOG
# define READ_MEM(addr, size) ({})
#else
# define READ_MEM(addr, size) \
  proc->state.log_mem_read.push_back(std::make_tuple(addr, 0, size));
#endif

#define RISCV_XLATE_VIRT (1U << 0)
#define RISCV_XLATE_VIRT_MXR (1U << 1)

  // template for functions that load an aligned value from memory
  #define load_func(type, prefix, xlate_flags) \
    inline type##_t prefix##_##type(reg_t addr, bool require_alignment = false) { \
      if ((xlate_flags) != 0) \
        flush_tlb(); \
      if (unlikely(addr & (sizeof(type##_t)-1))) { \
        if (require_alignment) load_reserved_address_misaligned(addr); \
        else return misaligned_load(addr, sizeof(type##_t)); \
      } \
      reg_t vpn = addr >> PGSHIFT; \
      size_t size = sizeof(type##_t); \
      if (likely(tlb_load_tag[vpn % TLB_ENTRIES] == vpn)) { \
        if (proc) READ_MEM(addr, size); \
        return from_target(*(target_endian<type##_t>*)(tlb_data[vpn % TLB_ENTRIES].host_offset + addr)); \
      } \
      if (unlikely(tlb_load_tag[vpn % TLB_ENTRIES] == (vpn | TLB_CHECK_TRIGGERS))) { \
        type##_t data = from_target(*(target_endian<type##_t>*)(tlb_data[vpn % TLB_ENTRIES].host_offset + addr)); \
        if (!matched_trigger) { \
          matched_trigger = trigger_exception(OPERATION_LOAD, addr, data); \
          if (matched_trigger) \
            throw *matched_trigger; \
        } \
        if (proc) READ_MEM(addr, size); \
        return data; \
      } \
      target_endian<type##_t> res; \
      load_slow_path(addr, sizeof(type##_t), (uint8_t*)&res, (xlate_flags)); \
      if (proc) READ_MEM(addr, size); \
      if ((xlate_flags) != 0) \
        flush_tlb(); \
      return from_target(res); \
    }

  // load value from memory at aligned address; zero extend to register width
  load_func(uint8, load, 0)
  load_func(uint16, load, 0)
  load_func(uint32, load, 0)
  load_func(uint64, load, 0)

  // load value from guest memory at aligned address; zero extend to register width
  load_func(uint8, guest_load, RISCV_XLATE_VIRT)
  load_func(uint16, guest_load, RISCV_XLATE_VIRT)
  load_func(uint32, guest_load, RISCV_XLATE_VIRT)
  load_func(uint64, guest_load, RISCV_XLATE_VIRT)
  load_func(uint16, guest_load_x, RISCV_XLATE_VIRT|RISCV_XLATE_VIRT_MXR)
  load_func(uint32, guest_load_x, RISCV_XLATE_VIRT|RISCV_XLATE_VIRT_MXR)

  // load value from memory at aligned address; sign extend to register width
  load_func(int8, load, 0)
  load_func(int16, load, 0)
  load_func(int32, load, 0)
  load_func(int64, load, 0)

  // load value from guest memory at aligned address; sign extend to register width
  load_func(int8, guest_load, RISCV_XLATE_VIRT)
  load_func(int16, guest_load, RISCV_XLATE_VIRT)
  load_func(int32, guest_load, RISCV_XLATE_VIRT)
  load_func(int64, guest_load, RISCV_XLATE_VIRT)

#ifndef RISCV_ENABLE_COMMITLOG
# define WRITE_MEM(addr, value, size) ({})
#else
# define WRITE_MEM(addr, val, size) \
  proc->state.log_mem_write.push_back(std::make_tuple(addr, val, size));
#endif

  // template for functions that store an aligned value to memory
  #define store_func(type, prefix, xlate_flags) \
    void prefix##_##type(reg_t addr, type##_t val) { \
      if ((xlate_flags) != 0) \
        flush_tlb(); \
      if (unlikely(addr & (sizeof(type##_t)-1))) \
        return misaligned_store(addr, val, sizeof(type##_t)); \
      reg_t vpn = addr >> PGSHIFT; \
      size_t size = sizeof(type##_t); \
      if (likely(tlb_store_tag[vpn % TLB_ENTRIES] == vpn)) { \
        if (proc) WRITE_MEM(addr, val, size); \
        *(target_endian<type##_t>*)(tlb_data[vpn % TLB_ENTRIES].host_offset + addr) = to_target(val); \
      } \
      else if (unlikely(tlb_store_tag[vpn % TLB_ENTRIES] == (vpn | TLB_CHECK_TRIGGERS))) { \
        if (!matched_trigger) { \
          matched_trigger = trigger_exception(OPERATION_STORE, addr, val); \
          if (matched_trigger) \
            throw *matched_trigger; \
        } \
        if (proc) WRITE_MEM(addr, val, size); \
        *(target_endian<type##_t>*)(tlb_data[vpn % TLB_ENTRIES].host_offset + addr) = to_target(val); \
      } \
      else { \
        target_endian<type##_t> target_val = to_target(val); \
        store_slow_path(addr, sizeof(type##_t), (const uint8_t*)&target_val, (xlate_flags)); \
        if (proc) WRITE_MEM(addr, val, size); \
      } \
      if ((xlate_flags) != 0) \
        flush_tlb(); \
  }

  // template for functions that perform an atomic memory operation
  #define amo_func(type) \
    template<typename op> \
    type##_t amo_##type(reg_t addr, op f) { \
      try { \
        auto lhs = load_##type(addr, true); \
        store_##type(addr, f(lhs)); \
        return lhs; \
      } catch (trap_load_address_misaligned& t) { \
        /* AMO faults should be reported as store faults */ \
        throw trap_store_address_misaligned(t.get_tval(), t.get_tval2(), t.get_tinst()); \
      } catch (trap_load_page_fault& t) { \
        /* AMO faults should be reported as store faults */ \
        throw trap_store_page_fault(t.has_gva(), t.get_tval(), t.get_tval2(), t.get_tinst()); \
      } catch (trap_load_access_fault& t) { \
        /* AMO faults should be reported as store faults */ \
        throw trap_store_access_fault(t.has_gva(), t.get_tval(), t.get_tval2(), t.get_tinst()); \
      } catch (trap_load_guest_page_fault& t) { \
        /* AMO faults should be reported as store faults */ \
        throw trap_store_guest_page_fault(t.get_tval(), t.get_tval2(), t.get_tinst()); \
      } \
    }

  void store_float128(reg_t addr, float128_t val)
  {
#ifndef RISCV_ENABLE_MISALIGNED
    if (unlikely(addr & (sizeof(float128_t)-1)))
      throw trap_store_address_misaligned(addr, 0, 0);
#endif
    store_uint64(addr, val.v[0]);
    store_uint64(addr + 8, val.v[1]);
  }

  float128_t load_float128(reg_t addr)
  {
#ifndef RISCV_ENABLE_MISALIGNED
    if (unlikely(addr & (sizeof(float128_t)-1)))
      throw trap_load_address_misaligned(addr, 0, 0);
#endif
    return (float128_t){load_uint64(addr), load_uint64(addr + 8)};
  }

  // store value to memory at aligned address
  store_func(uint8, store, 0)
  store_func(uint16, store, 0)
  store_func(uint32, store, 0)
  store_func(uint64, store, 0)

  // store value to guest memory at aligned address
  store_func(uint8, guest_store, RISCV_XLATE_VIRT)
  store_func(uint16, guest_store, RISCV_XLATE_VIRT)
  store_func(uint32, guest_store, RISCV_XLATE_VIRT)
  store_func(uint64, guest_store, RISCV_XLATE_VIRT)

  // perform an atomic memory operation at an aligned address
  amo_func(uint32)
  amo_func(uint64)

  inline void yield_load_reservation()
  {
    load_reservation_address = (reg_t)-1;
  }

  inline void acquire_load_reservation(reg_t vaddr)
  {
    reg_t paddr = translate(vaddr, 1, LOAD, 0);
    if (auto host_addr = sim->addr_to_mem(paddr))
      load_reservation_address = refill_tlb(vaddr, paddr, host_addr, LOAD).target_offset + vaddr;
    else
      throw trap_load_access_fault((proc) ? proc->state.v : false, vaddr, 0, 0); // disallow LR to I/O space
  }

  inline void load_reserved_address_misaligned(reg_t vaddr)
  {
#ifdef RISCV_ENABLE_MISALIGNED
    throw trap_load_access_fault((proc) ? proc->state.v : false, vaddr, 0, 0);
#else
    throw trap_load_address_misaligned(vaddr, 0, 0);
#endif
  }

  inline void store_conditional_address_misaligned(reg_t vaddr)
  {
#ifdef RISCV_ENABLE_MISALIGNED
    throw trap_store_access_fault((proc) ? proc->state.v : false, vaddr, 0, 0);
#else
    throw trap_store_address_misaligned(vaddr, 0, 0);
#endif
  }

  inline bool check_load_reservation(reg_t vaddr, size_t size)
  {
    if (vaddr & (size-1))
      store_conditional_address_misaligned(vaddr);

    reg_t paddr = translate(vaddr, 1, STORE, 0);
    if (auto host_addr = sim->addr_to_mem(paddr))
      return load_reservation_address == refill_tlb(vaddr, paddr, host_addr, STORE).target_offset + vaddr;
    else
      throw trap_store_access_fault((proc) ? proc->state.v : false, vaddr, 0, 0); // disallow SC to I/O space
  }

  static const reg_t ICACHE_ENTRIES = 1024;

  inline size_t icache_index(reg_t addr)
  {
    return (addr / PC_ALIGN) % ICACHE_ENTRIES;
  }

  inline icache_entry_t* refill_icache(reg_t addr, icache_entry_t* entry)
  {
    auto tlb_entry = translate_insn_addr(addr);
    insn_bits_t insn = from_le(*(uint16_t*)(tlb_entry.host_offset + addr));
    int length = insn_length(insn);

    if (likely(length == 4)) {
      insn |= (insn_bits_t)from_le(*(const int16_t*)translate_insn_addr_to_host(addr + 2)) << 16;
    } else if (length == 2) {
      insn = (int16_t)insn;
    } else if (length == 6) {
      insn |= (insn_bits_t)from_le(*(const int16_t*)translate_insn_addr_to_host(addr + 4)) << 32;
      insn |= (insn_bits_t)from_le(*(const uint16_t*)translate_insn_addr_to_host(addr + 2)) << 16;
    } else {
      static_assert(sizeof(insn_bits_t) == 8, "insn_bits_t must be uint64_t");
      insn |= (insn_bits_t)from_le(*(const int16_t*)translate_insn_addr_to_host(addr + 6)) << 48;
      insn |= (insn_bits_t)from_le(*(const uint16_t*)translate_insn_addr_to_host(addr + 4)) << 32;
      insn |= (insn_bits_t)from_le(*(const uint16_t*)translate_insn_addr_to_host(addr + 2)) << 16;
    }

    insn_fetch_t fetch = {proc->decode_insn(insn), insn};
    entry->tag = addr;
    entry->next = &icache[icache_index(addr + length)];
    entry->data = fetch;

    reg_t paddr = tlb_entry.target_offset + addr;;
    if (tracer.interested_in_range(paddr, paddr + 1, FETCH)) {
      entry->tag = -1;
      tracer.trace(paddr, length, FETCH);
    }
    return entry;
  }

  inline icache_entry_t* access_icache(reg_t addr)
  {
    icache_entry_t* entry = &icache[icache_index(addr)];
    if (likely(entry->tag == addr))
      return entry;
    return refill_icache(addr, entry);
  }

  inline insn_fetch_t load_insn(reg_t addr)
  {
    icache_entry_t entry;
    return refill_icache(addr, &entry)->data;
  }

  void flush_tlb();
  void flush_icache();

  void register_memtracer(memtracer_t*);

  int is_dirty_enabled()
  {
#ifdef RISCV_ENABLE_DIRTY
    return 1;
#else
    return 0;
#endif
  }

  int is_misaligned_enabled()
  {
#ifdef RISCV_ENABLE_MISALIGNED
    return 1;
#else
    return 0;
#endif
  }

  void set_target_big_endian(bool enable, bool enable_for_pte)
  {
#ifdef RISCV_ENABLE_DUAL_ENDIAN
    target_big_endian = enable;
    target_pte_big_endian = enable_for_pte;
#else
    assert(enable == false);
    assert(enable_for_pte == false);
#endif
  }

  void set_target_big_endian(bool enable) {
    set_target_big_endian(enable, enable);
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
  uint16_t fetch_temp;

  // implement an instruction cache for simulator performance
  icache_entry_t icache[ICACHE_ENTRIES];

  // implement a TLB for simulator performance
  static const reg_t TLB_ENTRIES = 256;
  // If a TLB tag has TLB_CHECK_TRIGGERS set, then the MMU must check for a
  // trigger match before completing an access.
  static const reg_t TLB_CHECK_TRIGGERS = reg_t(1) << 63;
  tlb_entry_t tlb_data[TLB_ENTRIES];
  reg_t tlb_insn_tag[TLB_ENTRIES];
  reg_t tlb_load_tag[TLB_ENTRIES];
  reg_t tlb_store_tag[TLB_ENTRIES];

  // finish translation on a TLB miss and update the TLB
  tlb_entry_t refill_tlb(reg_t vaddr, reg_t paddr, char* host_addr, access_type type);
  const char* fill_from_mmio(reg_t vaddr, reg_t paddr);

  // perform a stage2 translation for a given guest address
  reg_t s2xlate(reg_t gva, reg_t gpa, access_type type, access_type trap_type, bool virt, bool mxr);

  // perform a page table walk for a given VA; set referenced/dirty bits
  reg_t walk(reg_t addr, access_type type, reg_t prv, bool virt, bool mxr);

  // handle uncommon cases: TLB misses, page faults, MMIO
  tlb_entry_t fetch_slow_path(reg_t addr);
  void load_slow_path(reg_t addr, reg_t len, uint8_t* bytes, uint32_t xlate_flags);
  void store_slow_path(reg_t addr, reg_t len, const uint8_t* bytes, uint32_t xlate_flags);
  bool mmio_load(reg_t addr, size_t len, uint8_t* bytes);
  bool mmio_store(reg_t addr, size_t len, const uint8_t* bytes);
  bool mmio_ok(reg_t addr, access_type type);
  reg_t translate(reg_t addr, reg_t len, access_type type, uint32_t xlate_flags);

  // ITLB lookup
  inline tlb_entry_t translate_insn_addr(reg_t addr) {
    reg_t vpn = addr >> PGSHIFT;
    if (likely(tlb_insn_tag[vpn % TLB_ENTRIES] == vpn))
      return tlb_data[vpn % TLB_ENTRIES];
    tlb_entry_t result;
    if (unlikely(tlb_insn_tag[vpn % TLB_ENTRIES] != (vpn | TLB_CHECK_TRIGGERS))) {
      result = fetch_slow_path(addr);
    } else {
      result = tlb_data[vpn % TLB_ENTRIES];
    }
    if (unlikely(tlb_insn_tag[vpn % TLB_ENTRIES] == (vpn | TLB_CHECK_TRIGGERS))) {
      target_endian<uint16_t>* ptr = (target_endian<uint16_t>*)(tlb_data[vpn % TLB_ENTRIES].host_offset + addr);
      int match = proc->trigger_match(OPERATION_EXECUTE, addr, from_target(*ptr));
      if (match >= 0) {
        throw trigger_matched_t(match, OPERATION_EXECUTE, addr, from_target(*ptr));
      }
    }
    return result;
  }

  inline const uint16_t* translate_insn_addr_to_host(reg_t addr) {
    return (uint16_t*)(translate_insn_addr(addr).host_offset + addr);
  }

  inline trigger_matched_t *trigger_exception(trigger_operation_t operation,
      reg_t address, reg_t data)
  {
    if (!proc) {
      return NULL;
    }
    int match = proc->trigger_match(operation, address, data);
    if (match == -1)
      return NULL;
    if (proc->state.mcontrol[match].timing == 0) {
      throw trigger_matched_t(match, operation, address, data);
    }
    return new trigger_matched_t(match, operation, address, data);
  }

  template<typename T> inline T pte_from_target(target_endian<T> n) const
  {
    return target_pte_big_endian? n.from_be() : n.from_le();
  }

  template<typename T> inline target_endian<T> pte_to_target(T n) const
  {
    return target_pte_big_endian? target_endian<T>::to_be(n) : target_endian<T>::to_le(n);
  }

  reg_t pmp_homogeneous(reg_t addr, reg_t len);
  reg_t pmp_ok(reg_t addr, reg_t len, access_type type, reg_t mode);

#ifdef RISCV_ENABLE_DUAL_ENDIAN
  bool target_big_endian;
  bool target_pte_big_endian;
#else
  static const bool target_big_endian = false;
  static const bool target_pte_big_endian = false;
#endif
  bool check_triggers_fetch;
  bool check_triggers_load;
  bool check_triggers_store;
  // The exception describing a matched trigger, or NULL.
  trigger_matched_t *matched_trigger;

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
      default: abort();
    }
  } else {
    abort();
  }
}

#endif
