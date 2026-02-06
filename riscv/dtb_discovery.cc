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
      node_name.c_str(),
      device_name.c_str(),
      (parsed_args.empty() ? "" : join_csv(parsed_args).c_str()));

      factories.push_back({factory, parsed_args});
    }
  }
}

void discover_memory_from_dtb(const void* fdt, std::vector<std::pair<reg_t, abstract_mem_t*>>& mems) {
  int offset = -1;
  while ((offset = fdt_node_offset_by_compatible(fdt, offset, "memory")) >= 0) {
    const char* node_name = fdt_get_name(fdt, offset, NULL);
    auto parsed_args = get_device_params(fdt, offset);

    reg_t base, size;
    try {
        base = std::stoull(parsed_args[0], nullptr, 0);
        size = std::stoull(parsed_args[1], nullptr, 0);
    } catch (const std::exception& e) {
        fprintf(stderr, "Error parsing memory params: %s\n", e.what());
        continue;
    } 

    fprintf(stdout, "DTB memory device: %s - compatible with device type: memory - params: %s - Memory initialized at [%" PRIx64 ", %" PRIx64 ")\n",
      node_name.c_str(),
      (parsed_args.empty() ? "" : join_csv(parsed_args).c_str()),
      base,
      base + size);

    auto mem=std::make_pair(base, new mem_t(size));
    mems.push_back(mem);
  }
}
} // namespace dtb_discovery
