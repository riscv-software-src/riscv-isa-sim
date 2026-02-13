#include <riscv/sim.h>
#include <riscv/dtb_discovery.h>
#include <riscv/devices.h>
#include <riscv/mmu.h>
#include <libfdt.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <cstring>
#include <dlfcn.h>

#include "led-device.h"

struct expected_mem_t {
  reg_t base;
};

struct expected_device_t {
  const std::type_info* compatible;
  reg_t base;
};

struct test_case_t {
  std::vector<expected_mem_t> memory;
  std::vector<expected_device_t> devices;
};

// Key: pair of (filename, dtb_discovery_enabled)
using test_key_t = std::pair<std::string, bool>;

static std::map<test_key_t, test_case_t> test_cases_expected_results = {
  // With DTB discovery enabled
  { {"mem-64bit-single.dtb", true}, {
      {{0x20000000ULL}},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-64bit-single.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-32bit.dtb", true}, {
      {{0x80000000ULL}},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-32bit.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-multiple-nodes.dtb", true}, {
      {{0x80000000ULL}, {0xA0000000ULL}},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-multiple-nodes.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-mixed-cells-sizes.dtb", true}, {
      {{0x100000000ULL}},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"mem-mixed-cells-sizes.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"devices-single-led.dtb", true}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL},
       {&typeid(led_device_t), 0x10001000ULL}}
  }},
  { {"devices-single-led.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"devices-multiple-leds.dtb", true}, {
      {{0x90000000ULL}},
      {{&typeid(debug_module_t), 0x0ULL},
       {&typeid(led_device_t), 0x10001000ULL},
       {&typeid(led_device_t), 0x10002000ULL}}
  }},
  { {"devices-multiple-leds.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL}}
  }},
  { {"devices-clint-plic-uart.dtb", true}, {
      {{0x90000000ULL}},
      {{&typeid(debug_module_t), 0x0ULL},
       {&typeid(clint_t), 0x2000000ULL},
       {&typeid(plic_t), 0xc000000ULL},
       {&typeid(ns16550_t), 0x10000000ULL}}
  }},
  { {"devices-clint-plic-uart.dtb", false}, {
      {},
      {{&typeid(debug_module_t), 0x0ULL},
       {&typeid(clint_t), 0x2000000ULL},
       {&typeid(plic_t), 0xc000000ULL},
       {&typeid(ns16550_t), 0x10000000ULL}}
  }}
};

bool check_memory_devices(const std::map<reg_t, abstract_device_t*>& devices,
                          const std::vector<expected_mem_t>& expected_memory) {
  bool passed = true;

  for (const auto& mem : expected_memory) {
    auto it = devices.find(mem.base);

    if (it == devices.end()) {
      std::cerr << "Memory NOT found at 0x" << std::hex << mem.base << std::endl;
      passed = false;
    } else if (dynamic_cast<mem_t*>(it->second) == nullptr) {
      std::cerr << "Device at 0x" << std::hex << mem.base
                << " is not mem_t type" << std::endl;
      passed = false;
    } else {
      std::cout << "Memory found at 0x" << std::hex << mem.base << std::endl;
    }
  }

  return passed;
}

bool check_devices(const std::map<reg_t, abstract_device_t*>& devices,
                          const std::vector<expected_device_t>& expected_devices) {
  bool passed = true;

  for (const auto& expected_device : expected_devices) {
    auto it = devices.find(expected_device.base);

    if (it == devices.end()) {
      std::cerr << "Expected device not found at 0x" << std::hex << expected_device.base << std::endl;
      passed = false;
      continue;
    }

    const std::type_info& actual_type = typeid(*it->second);

    if (*expected_device.compatible != actual_type) {
      std::cerr << "Device at 0x" << std::hex << expected_device.base
                << " has wrong type. Expected: " << expected_device.compatible->name()
                << ", Got: " << actual_type.name() << std::endl;
      passed = false;
    } else {
      std::cout << "Expected device found at 0x" << std::hex << expected_device.base
                << " Type: " << actual_type.name() << std::endl;
    }
  }

  return passed;
}

bool check_no_unexpected_devices(const std::map<reg_t, abstract_device_t*>& devices,
                                 const test_case_t& expected) {
  size_t expected_count = expected.memory.size() + expected.devices.size();

  if (devices.size() != expected_count) {
    std::cerr << "Device count mismatch. Expected: " << expected_count
              << ", Got: " << devices.size() << std::endl;
    return false;
  }

  return true;
}

bool compare_devices(sim_t& sim, const test_case_t& expected) {
  const auto& devices = sim.get_bus().get_devices();

  bool no_extra_ok = check_no_unexpected_devices(devices, expected);
  bool memory_ok = check_memory_devices(devices, expected.memory);
  bool devices_ok= check_devices(devices, expected.devices);

  return memory_ok && devices_ok && no_extra_ok;
}

bool run_test(const std::string& dtb_path, bool dtb_discovery,
              const std::string& pk, const std::string& executable) {
  // Extract filename from dtb path
  std::filesystem::path p(dtb_path);
  std::string dtb_filename = p.filename().string();

  // Create test key
  test_key_t test_key = {dtb_filename, dtb_discovery};

  // Find expected layout
  auto test_it = test_cases_expected_results.find(test_key);
  if (test_it == test_cases_expected_results.end()) {
    std::cout << "Error: No expected result for '" << dtb_filename
              << "' with dtb_discovery=" << (dtb_discovery ? "true" : "false") << "\n";
    return false;
  }

  const auto& expected = test_it->second;
  std::cout << "Testing: " << dtb_filename << "\n";
  std::cout << "DTB Discovery: " << (dtb_discovery ? "enabled" : "disabled") << "\n";

  // Setup simulator
  cfg_t cfg;
  std::vector<device_factory_sargs_t> devices;
  std::vector<std::string> htif_args{pk, executable};
  debug_module_config_t dm_config;
  std::vector<std::pair<reg_t, abstract_mem_t*>> mems;

  sim_t sim(&cfg, false,
            mems,
            devices,
            dtb_discovery,
            htif_args,
            dm_config,
            nullptr,
            true,
            dtb_path.c_str(),
            false,
            nullptr,
            std::nullopt);

  // Compare devices
  bool result = compare_devices(sim, expected);

  if (result) {
    std::cout << "✓ TEST PASSED: " << dtb_filename
              << " (dtb_discovery=" << (dtb_discovery ? "enabled" : "disabled") << ")\n\n";
  } else {
    std::cout << "✗ TEST FAILED: " << dtb_filename
              << " (dtb_discovery=" << (dtb_discovery ? "enabled" : "disabled") << ")\n\n";
  }

  return result;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " --dtb=<file> <pk> <executable>\n";
    return 1;
  }

  std::string dtb_path;
  std::string pk;
  std::string executable;

  // Parse arguments
  int arg_idx = 1;

  // First argument must be --dtb=
  std::string dtb_arg = argv[arg_idx++];
  if (dtb_arg.find("--dtb=") == 0) {
    dtb_path = dtb_arg.substr(strlen("--dtb="));
  } else {
    std::cerr << "Error: First argument must be --dtb=<file>\n";
    return 1;
  }

  // Remaining arguments are pk and executable
  pk = argv[arg_idx++];
  executable = argv[arg_idx];

  // Run tests with both dtb_discovery values
  bool test_dtb_discovery_disabled = run_test(dtb_path, false, pk, executable);
  bool test_dtb_discovery_enabled = run_test(dtb_path, true, pk, executable);

  return !(test_dtb_discovery_disabled && test_dtb_discovery_enabled);
}
