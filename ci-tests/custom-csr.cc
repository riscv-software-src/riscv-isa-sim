#include <riscv/extension.h>
#include <riscv/sim.h>


class dummycsr_t: public csr_t {
  public:
  dummycsr_t(processor_t *proc, const reg_t addr): csr_t(proc, addr) {}

  reg_t read() const noexcept override {
    return 42;
  }

  void verify_permissions(insn_t insn, bool write) const override {}

  protected:
  bool unlogged_write(const reg_t val) noexcept override {
    return true;
  }
};

// dummy extension with dummy CSRs. Nice.
struct xdummycsr_t : public extension_t {
  const char *name() const override { return "dummycsr"; }

  xdummycsr_t() {}

  std::vector<insn_desc_t> get_instructions(const processor_t &) override {
    return {};
  }

  std::vector<disasm_insn_t *> get_disasms(const processor_t *) override {
    return {};
  }

  std::vector<csr_t_p> get_csrs(processor_t &proc) const override {
    return {std::make_shared<dummycsr_t>(&proc, /*Addr*/ 0xfff)};
  }
};

REGISTER_EXTENSION(dummycsr, []() { return new xdummycsr_t; })

// Copied from spike main.
// TODO: This should really be provided in libriscv
static std::vector<std::pair<reg_t, abstract_mem_t *>>
make_mems(const std::vector<mem_cfg_t> &layout) {
  std::vector<std::pair<reg_t, abstract_mem_t *>> mems;
  mems.reserve(layout.size());
  for (const auto &cfg : layout) {
    mems.push_back(std::make_pair(cfg.get_base(), new mem_t(cfg.get_size())));
  }
  return mems;
}

int main(int argc, char **argv) {
  cfg_t cfg;
  cfg.isa = "RV64IMAFDCV_Zicsr_xdummycsr";
  std::vector<device_factory_sargs_t> plugin_devices;
  if (argc != 3) {
    std::cerr << "Error: invalid arguments\n";
    exit(1);
  }
  std::vector<std::string> htif_args{argv[1] /* pk */, argv[2] /* executable */};
  debug_module_config_t dm_config = {.progbufsize = 2,
                                     .max_sba_data_width = 0,
                                     .require_authentication = false,
                                     .abstract_rti = 0,
                                     .support_hasel = true,
                                     .support_abstract_csr_access = true,
                                     .support_abstract_fpr_access = true,
                                     .support_haltgroups = true,
                                     .support_impebreak = true};
  std::vector<std::pair<reg_t, abstract_mem_t *>> mems =
      make_mems(cfg.mem_layout);
  sim_t sim(&cfg, false, mems, plugin_devices, htif_args, dm_config,
            nullptr,  // log_path
            true,     // dtb_enabled
            nullptr,  // dtb_file
            false,    // socket_enabled
            nullptr,  // cmd_file
            std::nullopt); // instruction_limit
  sim.run();
}
