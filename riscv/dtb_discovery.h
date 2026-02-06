#ifndef _RISCV_DTB_DISCOVERY_H
#define _RISCV_DTB_DISCOVERY_H

#include "sim.h"
#include <vector>
#include <string>

class bus_t;
class abstract_mem_t;

namespace dtb_discovery {

void discover_devices_from_dtb(const void* fdt,
                               std::vector<device_factory_sargs_t>& factories);

void discover_memory_from_dtb(const void* fdt, std::vector<std::pair<reg_t, abstract_mem_t*>>& mems);

} // namespace dtb_discovery


#endif // _RISCV_DTB_DISCOVERY_H
