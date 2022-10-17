// See LICENSE for license details.
#ifndef _RISCV_DTS_H
#define _RISCV_DTS_H

#include "devices.h"
#include "processor.h"
#include "mmu.h"
#include <string>

std::string make_dts(size_t insns_per_rtc_tick, size_t cpu_hz,
                     reg_t initrd_start, reg_t initrd_end,
                     const char* bootargs,
                     size_t pmpregions,
                     std::vector<processor_t*> procs,
                     std::vector<std::pair<reg_t, mem_t*>> mems);

std::string dts_compile(const std::string& dts);

int fdt_get_offset(void *fdt, const char *field);
int fdt_get_first_subnode(void *fdt, int node);
int fdt_get_next_subnode(void *fdt, int node);

int fdt_parse_clint(void *fdt, reg_t *clint_addr,
                    const char *compatible);
int fdt_parse_plic(void *fdt, reg_t *plic_addr, uint32_t *ndev,
                   const char *compatible);
int fdt_parse_ns16550(void *fdt, reg_t *ns16550_addr,
                      uint32_t *reg_shift, uint32_t *reg_io_width,
                      const char *compatible);
int fdt_parse_pmp_num(void *fdt, int cpu_offset, reg_t *pmp_num);
int fdt_parse_pmp_alignment(void *fdt, int cpu_offset, reg_t *pmp_align);
int fdt_parse_mmu_type(void *fdt, int cpu_offset, const char **mmu_type);
#endif
