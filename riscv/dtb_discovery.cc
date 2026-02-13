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

  // Helper to parse reg property
  bool parse_reg_property(const void* fdt, int offset, const char* node_name,
                          uint64_t& base, uint64_t& size) {
    int len = 0;
    const fdt32_t* reg_prop = (const fdt32_t*)fdt_getprop(fdt, offset, "reg", &len);
    if (!reg_prop) {
      fprintf(stderr, "DTB node '%s' missing 'reg' property\n", node_name);
      return false;
    }

    int addr_cells = fdt_address_cells(fdt, 0);
    int size_cells = fdt_size_cells(fdt, 0);
    if (addr_cells < 1 || size_cells < 1) {
      fprintf(stderr, "Invalid #address-cells or #size-cells for node '%s'\n", node_name);
      return false;
    }

    int entry_cells = addr_cells + size_cells;
    int entry_size = entry_cells * sizeof(fdt32_t);
    if ((len % entry_size) != 0) {
      fprintf(stderr, "DTB node '%s' has malformed 'reg'\n", node_name);
      return false;
    }

    const fdt32_t* p = reg_prop;
    base = 0;
    size = 0;

    // Parse base address
    for (int i = 0; i < addr_cells; ++i)
      base = (base << 32) | fdt32_to_cpu(p[i]);

    // Parse size
    for (int i = 0; i < size_cells; ++i)
      size = (size << 32) | fdt32_to_cpu(p[addr_cells + i]);

    return true;
  }

  // Core platform devices handled by Spike internally
  const std::set<std::string> SPIKE_DEFAULT_DEVICES = {
    "riscv,plic0",
    "sifive,plic-1.0.0",
    "ns16550a",
    "riscv,clint0"
  };

  bool should_skip_device(const std::string& device_name) {
    return SPIKE_DEFAULT_DEVICES.find(device_name) != SPIKE_DEFAULT_DEVICES.end();
  }
} // namespace

namespace dtb_discovery {

void discover_devices_from_dtb(const void* fdt,
                               std::vector<device_factory_sargs_t>& factories) {
  for (const auto& [device_name, factory] : mmio_device_map()) {
    //Spike default devices are not managed by dtb discovery feature
    if (should_skip_device(device_name)) continue;

    int offset = -1;
    while ((offset = fdt_node_offset_by_compatible(fdt, offset, device_name.c_str())) >= 0) {
      const char* node_name = fdt_get_name(fdt, offset, NULL);

      uint64_t base = 0, size = 0;
      if (!parse_reg_property(fdt, offset, node_name, base, size)) {
        exit(1);
      }

      auto parsed_args = get_device_params(fdt, offset);

      // Prepend base and size to arguments
      std::vector<std::string> full_args;
      full_args.push_back(std::to_string(base));
      full_args.push_back(std::to_string(size));
      full_args.insert(full_args.end(), parsed_args.begin(), parsed_args.end());

      fprintf(stdout, "DTB discovered device: %s - type: %s - base: 0x%016" PRIx64
              " - size: 0x%016" PRIx64 " - params: %s\n",
              node_name, device_name.c_str(), base, size,
              (parsed_args.empty() ? "" : join_csv(parsed_args).c_str()));

      factories.push_back({factory, full_args});
    }
  }
}

void discover_memory_from_dtb(const void* fdt,
                              std::vector<std::pair<reg_t, abstract_mem_t*>>& mems) {
  int offset = -1;
  while ((offset = fdt_node_offset_by_prop_value(
              fdt, offset, "device_type", "memory", sizeof("memory"))) >= 0) {
    const char* node_name = fdt_get_name(fdt, offset, nullptr);

    uint64_t base = 0, size = 0;
    if (!parse_reg_property(fdt, offset, node_name, base, size)) {
      exit(1);
    }

    fprintf(stdout,
            "DTB memory device: %s - Memory initialized at [0x%016" PRIx64 ", 0x%016" PRIx64 ")\n",
            node_name, base, base + size);

    mems.emplace_back(base, new mem_t(size));
  }
}

} // namespace dtb_discovery
