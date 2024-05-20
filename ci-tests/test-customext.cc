#include <riscv/extension.h>
#include <riscv/sim.h>

struct : public arg_t {
  std::string to_string(insn_t insn) const { return xpr_name[insn.rd()]; }
} xrd;

struct : public arg_t {
  std::string to_string(insn_t insn) const { return xpr_name[insn.rs1()]; }
} xrs1;

struct : public arg_t {
  std::string to_string(insn_t insn) const { return xpr_name[insn.rs2()]; }
} xrs2;

struct : public arg_t {
  std::string to_string(insn_t insn) const {
    return std::to_string((int)insn.shamt());
  }
} shamt;

static reg_t do_nop4([[maybe_unused]] processor_t *p,
                     [[maybe_unused]] insn_t insn, reg_t pc) {
  return pc + 4;
}

// dummy extension that uses the same prefix as standard zba extension
struct xslliuw_dummy_t : public extension_t {
  const char *name() { return "dummyslliuw"; }

  xslliuw_dummy_t() {}

  std::vector<insn_desc_t> get_instructions() {
    std::vector<insn_desc_t> insns;
    insns.push_back(insn_desc_t{MATCH_SLLI_UW, MASK_SLLI_UW, do_nop4, do_nop4,
                                do_nop4, do_nop4, do_nop4, do_nop4, do_nop4,
                                do_nop4});
    return insns;
  }

  std::vector<disasm_insn_t *> get_disasms() {
    std::vector<disasm_insn_t *> insns;
    insns.push_back(new disasm_insn_t("dummy_slliuw", MATCH_SLLI_UW,
                                      MASK_SLLI_UW, {&xrd, &xrs1, &shamt}));
    return insns;
  }
};

REGISTER_EXTENSION(dummyslliuw, []() { return new xslliuw_dummy_t; })

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
  cfg.isa = "RV64IMAFDCV_xdummyslliuw";
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
            nullptr); // cmd_file
  sim.run();
}
