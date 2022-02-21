// See LICENSE for license details.
#ifndef _RISCV_DTS_H
#define _RISCV_DTS_H

#include "cfg.h"
#include "isa_parser.h"
#include <memory>
#include <string>

class cpu_dtb_t
{
public:
  cpu_dtb_t(size_t cpu_idx, const void *dtb, int offset);

  reg_t get_num_pmp_regions() const { return num_pmp_regions; }
  reg_t get_pmp_granularity() const { return pmp_granularity; }
  impl_extension_t get_mmu_type() const { return mmu_type; }
  const isa_parser_t &get_isa() const { return *isa_parser; }

  // Check that this CPU node parsed from a device tree matches the base
  // configuration. i is the index of the current CPU (used in error messages).
  void check_compatible(size_t i, const cfg_t &cfg) const;

private:
  const void *dtb;
  int offset;

  reg_t num_pmp_regions;
  reg_t pmp_granularity;

  std::unique_ptr<isa_parser_t> isa_parser;

  impl_extension_t mmu_type;
};

class devicetree_t
{
public:
  devicetree_t(std::string dtb, std::string dts, const char *src_path);

  // Factory method that either loads a DTB file from dtb_path (if non-null) or
  // otherwise generates a standardised layout by generating a standardised DTS
  // file and then compiling it to DTB.
  static devicetree_t make(const char *dtb_path,
                           size_t insns_per_rtc_tick, size_t cpu_hz,
                           const cfg_t &cfg);

  // Check that this device tree is compatible with the configuration in cfg.
  // This is used when we have a DTB file and also some commandline arguments
  // and we want to check that they agree with each other.
  void check_compatible(const cfg_t &cfg) const;

  // Get CPU vector
  const std::vector<cpu_dtb_t> &get_cpus() const { return cpus; }

  // Search DTB for a CLINT. On success, writes its address to the non-null
  // clint_addr and returns 0. On failure, returns a negative libfdt error
  // code.
  int find_clint(reg_t *clint_addr, const char *compatible) const;

  // Get access to the DTS form of the device tree. This is empty if the device
  // tree was constructed directly from a DTB.
  const char *get_dts() const { return dts.c_str(); }

  // Get access to the DTB form of the device tree. This will always be
  // nonempty and has already passed the fdt_check_header() well-formedness
  // check.
  const void *get_dtb() const { return dtb.c_str(); }

  // Get access to the DTB in string form. This is useful if you want to know
  // its size in advance
  const std::string &get_dtb_str() const { return dtb; }

private:
  std::string dtb;
  std::string dts;

  std::vector<cpu_dtb_t> cpus;
};

#endif
