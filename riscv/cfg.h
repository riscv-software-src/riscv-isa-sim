// See LICENSE for license details.
#ifndef _RISCV_CFG_H
#define _RISCV_CFG_H

#include <optional>
#include <vector>
#include "decode.h"
#include <cassert>
#include <map>
class abstract_sim_if_t;

typedef enum {
  endianness_little,
  endianness_big
} endianness_t;

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
class mem_cfg_t
{
public:
  static bool check_if_supported(reg_t base, reg_t size);

  mem_cfg_t(reg_t base, reg_t size);

  reg_t get_base() const {
    return base;
  }

  reg_t get_size() const {
    return size;
  }

  reg_t get_inclusive_end() const {
    return base + size - 1;
  }

private:
  reg_t base;
  reg_t size;
};

class start_pc_t
{
public:
  void set_global(reg_t pc);

  void set_override(size_t hart_id, reg_t pc);

  std::optional<reg_t> get(size_t hart_id) const;

private:
  std::optional<reg_t>    global_pc;
  std::map<size_t, reg_t> hart_pcs;
};

class cfg_t
{
public:
  cfg_t();

  std::pair<reg_t, reg_t> initrd_bounds;
  const char *            bootargs;
  const char *            isa;
  const char *            priv;
  endianness_t            endianness;
  reg_t                   pmpregions;
  reg_t                   pmpgranularity;
  std::vector<mem_cfg_t>  mem_layout;
  start_pc_t              start_pc;
  std::vector<size_t>     hartids;
  bool                    explicit_hartids;
  bool                    real_time_clint;
  bool                    wfi_as_nop;
  reg_t                   trigger_count;
  reg_t                   mcontrol6_napot_maskmax;
  reg_t                   cache_blocksz;
  // RISC-V Worlds platform parameters.  The masks are bit vectors
  // indexed by WID and are truncated to `nworlds` bits.
  reg_t                   nworlds;        // NWorlds, in [1, XLEN]
  reg_t                   pmwid;          // WID of M-mode; reset value of mwid
  reg_t                   pmwidlist;      // WIDs permitted in mwid (Smwid)
  reg_t                   pmlwidlist;     // WIDs permitted in mlwid (Smlwid)
  reg_t                   pmwiddeleg;     // WIDs delegatable via mwiddeleg (Smwiddeleg)
  bool                    pmwid_locked;   // whether mwid is locked out of reset
  std::optional<abstract_sim_if_t*> external_simulator;

  size_t nprocs() const { return hartids.size(); }
  size_t max_hartid() const { return hartids.back(); }
};

#endif
