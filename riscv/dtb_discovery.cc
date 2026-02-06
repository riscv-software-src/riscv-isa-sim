#include "dtb_discovery.h"
#include "libfdt.h"
#include "dts.h"
#include <iostream>
#include <sstream>
#include <cstdlib>


namespace {
  std::vector<std::string> split_csv(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
      result.push_back(item);
    }
    return result;
  }

  std::string join_csv(const std::vector<std::string>& vec) {
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i) {
      result += vec[i];
      if (i < vec.size() - 1) result += ",";
    }
    return result;
  }

  std::vector<std::string> get_device_params(const void* fdt, int offset) {
    int param_len;
    const char* params = (char*)fdt_getprop(fdt, offset, "spike,plugin-params", &param_len);
    return params ? split_csv(params) : std::vector<std::string>();
  }
} // namespace


namespace dtb_discovery {
void discover_devices_from_dtb(const void* fdt,
                               std::vector<device_factory_sargs_t>& factories) {
  for (const auto& [device_name, factory] : mmio_device_map()) {
    int offset = -1;
    while ((offset = fdt_node_offset_by_compatible(fdt, offset, device_name.c_str())) >= 0) {
      const char* node_name = fdt_get_name(fdt, offset, NULL);
      auto parsed_args = get_device_params(fdt, offset);

      fprintf(stdout, "DTB discovered device: %s - compatible with device type: %s - params: %s\n",
      node_name,
      device_name.c_str(),
      (parsed_args.empty() ? "" : join_csv(parsed_args).c_str()));

      factories.push_back({factory, parsed_args});
    }
  }
}

void discover_memory_from_dtb(const void* fdt,
                              std::vector<std::pair<reg_t, abstract_mem_t*>>& mems)
{
  int offset = -1;

  while ((offset = fdt_node_offset_by_prop_value(
              fdt, offset,
              "device_type", "memory", sizeof("memory"))) >= 0)
  {
    const char* node_name = fdt_get_name(fdt, offset, nullptr);

    int len = 0;
    const fdt32_t* reg_prop = (const fdt32_t*)fdt_getprop(fdt, offset, "reg", &len);
    if (!reg_prop) {
      fprintf(stderr, "DTB memory node '%s' missing 'reg' property\n", node_name);
      continue;
    }

    // Default per DT spec if not specified at root
    int addr_cells = fdt_address_cells(fdt, 0);
    int size_cells = fdt_size_cells(fdt, 0);

    if (addr_cells < 1 || size_cells < 1) {
      fprintf(stderr, "Invalid #address-cells or #size-cells for memory node '%s'\n", node_name);
      continue;
    }

    int entry_cells = addr_cells + size_cells;
    int entry_size = entry_cells * sizeof(fdt32_t);

    if ((len % entry_size)!=0) {
      fprintf(stderr, "DTB memory node '%s' has malformed 'reg'\n", node_name);
      continue;
    }

    const fdt32_t* p = reg_prop;

    uint64_t base = 0;
    uint64_t size = 0;

    // Parse base
    for (int i = 0; i < addr_cells; ++i)
      base = (base << 32) | fdt32_to_cpu(p[i]);

    // Parse size
    for (int i = 0; i < size_cells; ++i)
      size = (size << 32) | fdt32_to_cpu(p[addr_cells + i]);

    fprintf(stdout,
            "DTB memory device: %s - Memory initialized at [0x%016" PRIx64 ", 0x%016" PRIx64 ")\n",
            node_name,
            base,
            base + size);

    mems.emplace_back(base, new mem_t(size));
  }
}


} // namespace dtb_discovery
