#include <riscv/sim.h>

// Copied from spike main.
// TODO: This should really be provided in libriscv
static std::vector<std::pair<reg_t, abstract_mem_t*>> make_mems(const std::vector<mem_cfg_t> &layout)
{
  std::vector<std::pair<reg_t, abstract_mem_t*>> mems;
  mems.reserve(layout.size());
  for (const auto &cfg : layout) {
    mems.push_back(std::make_pair(cfg.get_base(), new mem_t(cfg.get_size())));
  }
  return mems;
}

int main(int argc, char **argv) {
  cfg_t cfg;
  std::vector<device_factory_sargs_t> plugin_devices;

  if (argc != 3) {
    std::cerr << "Error: invalid arguments\n";
    exit(1);
  }
  std::vector<std::string> htif_args{argv[1] /* pk */,
                                     argv[2] /* executable */};
  debug_module_config_t dm_config;
  std::vector<std::pair<reg_t, abstract_mem_t*>> mems =
      make_mems(cfg.mem_layout);
  sim_t sim(&cfg, false,
            mems,
            plugin_devices,
            htif_args,
            dm_config,
            nullptr,
            true,
            nullptr,
            false,
            nullptr);
  sim.run();
}
