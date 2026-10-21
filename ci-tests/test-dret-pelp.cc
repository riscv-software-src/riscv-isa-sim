#include <cstdio>
#include <cstdlib>
#include <memory>

#include <riscv/sim.h>
#include <riscv/processor.h>
#include <riscv/decode.h>
#include <riscv/encoding.h>

constexpr uint32_t DRET = 0x7b200073;

static bool check(bool cond, const char *what) {
  if (!cond)
    fprintf(stderr, "FAIL: %s\n", what);
  return cond;
}

static bool run_test(const char *isa) {
  cfg_t cfg;
  cfg.isa = isa;

  std::vector<device_factory_sargs_t> plugin_devices;
  std::vector<std::string> htif_args{"none"};

  std::vector<std::unique_ptr<mem_t>> owned_mems;
  std::vector<std::pair<reg_t, abstract_mem_t *>> mems;
  for (const auto &m : cfg.mem_layout) {
    owned_mems.push_back(std::make_unique<mem_t>(m.get_size()));
    mems.emplace_back(m.get_base(), owned_mems.back().get());
  }

  sim_t sim(&cfg, false, mems, plugin_devices, false, htif_args,
            debug_module_config_t{}, nullptr, true, nullptr, false, nullptr,
            std::nullopt);

  if (!mems.at(0).second->store(0, sizeof(DRET),
                                reinterpret_cast<const uint8_t *>(&DRET))) {
    fprintf(stderr, "FAIL: store dret\n");
    return false;
  }

  auto *proc = sim.get_core(size_t(0));
  auto *state = proc->get_state();
  auto base = cfg.mem_layout.at(0).get_base();

  state->mseccfg->write(MSECCFG_MLPE);
  state->debug_mode = true;
  state->pc = base;
  state->dpc->write(base);
  auto dcsr = set_field(state->dcsr->read(), DCSR_PRV, PRV_M);
  state->dcsr->write(set_field(dcsr, DCSR_PELP, elp_t::LP_EXPECTED));

  proc->step(1);

  bool ok = check(!state->debug_mode, "resumed");
  ok = ok && check(state->elp == elp_t::LP_EXPECTED, "elp restored from dcsr.pelp");
  return ok && check(state->dcsr->pelp == elp_t::NO_LP_EXPECTED, "dcsr.pelp cleared");
}

int main() {
  bool ok = true;
  for (const char *isa : {"RV32IMAFDC_Zicfilp", "RV64IMAFDC_Zicfilp"}) {
    printf("%s\n", isa);
    ok = run_test(isa) && ok;
  }

  if (!ok)
    return EXIT_FAILURE;
  printf("Executed successfully\n");
  return EXIT_SUCCESS;
}
