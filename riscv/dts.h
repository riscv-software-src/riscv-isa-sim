// See LICENSE for license details.
#ifndef _RISCV_DTS_H
#define _RISCV_DTS_H

#include "cfg.h"
#include "isa_parser.h"
#include <string>

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

  // A wrapper for fdt_path_offset: returns non-negative block offset of node
  // with the given path or a negative libfdt error code on failure.
  int get_offset(const char *path) const;

  // Search DTB for a CLINT. On success, writes its address to the non-null
  // clint_addr and returns 0. On failure, returns a negative libfdt error
  // code.
  int find_clint(reg_t *clint_addr, const char *compatible) const;

  // Get the number of PMP regions for CPU node at cpu_offset. On success,
  // writes the number to the non-null pmp_num and returns 0. On failure,
  // returns a negative libfdt error code.
  int get_pmp_num(int cpu_offset, reg_t *pmp_num) const;

  // Get PMP granularity for CPU node at cpu_offset. On success, writes the
  // number to the non-null pmp_align and returns 0. On failure, returns a
  // negative libfdt error code.
  int get_pmp_alignment(int cpu_offset, reg_t *pmp_align) const;

  // Get MMU type for CPU node at cpu_offset. On success, writes the pointer to
  // the property string to the non-null mmu_type and returns 0. On failure,
  // returns a negative libfdt error code.
  int get_mmu_type(int cpu_offset, const char **mmu_type) const;

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
};

int fdt_get_first_subnode(const void *fdt, int node);
int fdt_get_next_subnode(const void *fdt, int node);

#endif
