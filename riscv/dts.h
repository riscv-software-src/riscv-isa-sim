// See LICENSE for license details.
#ifndef _RISCV_DTS_H
#define _RISCV_DTS_H

#include "devices.h"
#include "processor.h"
#include <string>

std::string make_dts(size_t insns_per_rtc_tick, size_t cpu_hz,
                     const cfg_t* cfg,
                     std::vector<std::pair<reg_t, abstract_mem_t*>> mems,
                     std::string device_nodes);

std::string dts_to_dtb(const std::string& dtc_input);
std::string dtb_to_dts(const std::string& dtc_input);

int fdt_get_node_addr_size(const void *fdt, int node, reg_t *addr,
                           unsigned long *size, const char *field);
int fdt_get_offset(const void *fdt, const char *field);
int fdt_get_first_subnode(const void *fdt, int node);
int fdt_get_next_subnode(const void *fdt, int node);

int fdt_parse_clint(const void *fdt, reg_t *clint_addr,
                    const char *compatible);
int fdt_parse_plic(const void *fdt, reg_t *plic_addr, uint32_t *ndev,
                   const char *compatible);
int fdt_parse_ns16550(const void *fdt, reg_t *ns16550_addr,
                      uint32_t *reg_shift, uint32_t *reg_io_width, uint32_t* reg_int_id,
                      const char *compatible);
int fdt_parse_pmp_num(const void *fdt, int cpu_offset, reg_t *pmp_num);
int fdt_parse_pmp_alignment(const void *fdt, int cpu_offset, reg_t *pmp_align);
int fdt_parse_mmu_type(const void *fdt, int cpu_offset, const char **mmu_type);
int fdt_parse_isa(const void *fdt, int cpu_offset, const char **isa_str);
int fdt_parse_hartid(const void *fdt, int cpu_offset, uint32_t *hartid);
#endif
