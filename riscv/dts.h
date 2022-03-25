// See LICENSE for license details.
#ifndef _RISCV_DTS_H
#define _RISCV_DTS_H

#include "cfg.h"
#include "isa_parser.h"
#include <string>

std::string make_dts(size_t insns_per_rtc_tick, size_t cpu_hz, const cfg_t &cfg);

std::string dts_compile(const std::string& dts);

int fdt_get_offset(void *fdt, const char *field);
int fdt_get_first_subnode(void *fdt, int node);
int fdt_get_next_subnode(void *fdt, int node);

int fdt_parse_clint(void *fdt, reg_t *clint_addr,
                    const char *compatible);
int fdt_parse_pmp_num(void *fdt, int cpu_offset, reg_t *pmp_num);
int fdt_parse_pmp_alignment(void *fdt, int cpu_offset, reg_t *pmp_align);
int fdt_parse_mmu_type(void *fdt, int cpu_offset, const char **mmu_type);
#endif
