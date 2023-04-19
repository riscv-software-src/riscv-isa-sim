#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "sim.h"
#include "mmu.h"


extern "C" const size_t CONFIG_MSIZE = 2 * 1024 * 1024 * 1024UL;

std::vector<mem_cfg_t> parse_mem_layout(const char *arg);
std::vector<std::pair<reg_t, mem_t *>> make_mems(const std::vector<mem_cfg_t> &layout);

static sim_t *spike_init(const uint8_t *data, size_t size) {
  std::vector<std::string> difftest_htif_args;
  difftest_htif_args.push_back("");
  char mem_layout_str[100];
  sprintf(mem_layout_str, "0x%x:0x%lx", DRAM_BASE, CONFIG_MSIZE);
  auto memory_layout = parse_mem_layout(mem_layout_str);
  std::vector<std::pair<reg_t, abstract_device_t*>> difftest_plugin_devices{};
  auto const cfg = new cfg_t(
    // std::pair<reg_t, reg_t> default_initrd_bounds,
    std::make_pair(0, 0),
    // const char *default_bootargs,
    nullptr,
    // const char *default_isa,
    "RV64IMAFDC_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zknd_zkne_zknh_zksed_zksh_svinval",
    // const char *default_priv
    DEFAULT_PRIV,
    // const char *default_varch,
    DEFAULT_VARCH,
    // const bool default_misaligned,
    false,
    // const endianness_t default_endianness,
    endianness_little,
    // const reg_t default_pmpregions,
    0,
    // const std::vector<mem_cfg_t> &default_mem_layout,
    memory_layout,
    // const std::vector<size_t> default_hartids,
    std::vector<size_t>{0},
    // bool default_real_time_clint,
    false,
    // const reg_t default_trigger_count
    0
  );

  const debug_module_config_t difftest_dm_config = {
    .progbufsize = 2,
    .max_sba_data_width = 0,
    .require_authentication = false,
    .abstract_rti = 0,
    .support_hasel = true,
    .support_abstract_csr_access = true,
    .support_abstract_fpr_access = true,
    .support_haltgroups = false,
    .support_impebreak = false
  };
  sim_t *s = new sim_t(
    // const cfg_t *cfg,
    cfg,
    // bool halted,
    false,
    // std::vector<std::pair<reg_t, mem_t*>> mems
    make_mems(memory_layout),
    // std::vector<std::pair<reg_t, abstract_device_t*>> plugin_devices
    difftest_plugin_devices,
    // const std::vector<std::string>& args
    difftest_htif_args,
    // const debug_module_config_t &dm_config
    difftest_dm_config,
    // const char *log_path
#ifndef LOG_PATH
    nullptr,
#else
    LOG_PATH,
#endif // LOG_PATH
    //bool dtb_enabled, const char *dtb_file, bool socket_enabled, FILE *cmd_file
    false, nullptr, false, nullptr
  );

  // enable commit log
#ifdef LOG_PATH
  printf("commit log is enabled\n");
  s->configure_log(false, true);
#endif // LOG_PATH

  // set initialized states
  auto p = s->get_core(0UL);
  // set pc to 0x8000_0000
  p->get_state()->pc = 0x80000000;
  // initialize the memory with specified data
  auto mmu = p->get_mmu();
  for (size_t i = 0; i < size; i++) {
    mmu->store(DRAM_BASE + i, data[i]);
  }

  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  auto s = spike_init(data, size);
  for (int i = 0; i < 1000; i++) {
    s->step(1);
  }
  delete s;
  return 0;
}
