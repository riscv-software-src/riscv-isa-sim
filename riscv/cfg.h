// See LICENSE for license details.
#ifndef _RISCV_CFG_H
#define _RISCV_CFG_H

#include <optional>
#include "decode.h"
#include "mmu.h"
#include <cassert>

template <typename T>
class cfg_arg_t {
public:
  cfg_arg_t(T default_val)
    : value(default_val), was_set(false) {}

  bool overridden() const { return was_set; }

  T operator()() const { return value; }

  T operator=(const T v) {
    value = v;
    was_set = true;
    return value;
  }

private:
  T value;
  bool was_set;
};

// Configuration that describes a memory region
// Note: the current implementation does not support the size of 2^64
// (such a region still can be representsd as 2 adjacents mem_cfg_t objects).
struct mem_cfg_t {
  bool is_valid_region() const {
    // For a memory region to be valid it should have:
    // * size is a multiple of the minimum page size
    // * base is aligned to the minimum page size
    // * memory region should be non-empty
    // * the last accessible address should still be representable in a reg_t

    // [(base + size) > base] - means that we don't have 64-bit overflow
    // [(base + size) == 0] - allows for overflow, but only if we exactly
    // 1 bit short -  this is to allow regions like:
    //  { base = 0xffff_ffff_ffff_f000, size: 0x1000 }
    return (size % PGSIZE == 0) && (base % PGSIZE == 0) &&
      ((base + size > base) || (base + size == 0));
  }

  reg_t get_base() const {
    assert(is_valid_region());
    return base;
  }

  reg_t get_size() const {
    assert(is_valid_region());
    return size;
  }

  reg_t get_inclusive_end() const {
    assert(is_valid_region());
    return base + size - 1;
  }

  reg_t base;
  reg_t size;
};

class cfg_t
{
public:
  cfg_t(std::pair<reg_t, reg_t> default_initrd_bounds,
        const char *default_bootargs,
        const char *default_isa, const char *default_priv,
        const char *default_varch,
        const std::vector<mem_cfg_t> &default_mem_layout,
        const std::vector<int> default_hartids,
        bool default_real_time_clint)
    : initrd_bounds(default_initrd_bounds),
      bootargs(default_bootargs),
      isa(default_isa),
      priv(default_priv),
      varch(default_varch),
      mem_layout(default_mem_layout),
      hartids(default_hartids),
      explicit_hartids(false),
      real_time_clint(default_real_time_clint)
  {}

  cfg_arg_t<std::pair<reg_t, reg_t>> initrd_bounds;
  cfg_arg_t<const char *>            bootargs;
  cfg_arg_t<const char *>            isa;
  cfg_arg_t<const char *>            priv;
  cfg_arg_t<const char *>            varch;
  cfg_arg_t<std::vector<mem_cfg_t>>  mem_layout;
  std::optional<reg_t>               start_pc;
  cfg_arg_t<std::vector<int>>        hartids;
  bool                               explicit_hartids;
  cfg_arg_t<bool>                    real_time_clint;

  size_t nprocs() const { return hartids().size(); }
};

#endif
