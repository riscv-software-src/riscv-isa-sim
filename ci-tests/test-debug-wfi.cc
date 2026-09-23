#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

#include <riscv/sim.h>
#include <riscv/processor.h>
#include <riscv/decode.h>
#include <riscv/encoding.h>

// A WFI in Debug Mode must end step(), or the ROM's idle loop eats the whole budget.
constexpr uint32_t WFI = 0x10500073;
constexpr size_t BUDGET = 1000;

static bool check_eq(reg_t got, reg_t want, const char *what) {
  if (got == want)
    return true;
  fprintf(stderr, "FAIL: %s: got %" PRIu64 ", want %" PRIu64 "\n", what,
          static_cast<uint64_t>(got), static_cast<uint64_t>(want));
  return false;
}

static bool test_wfi_yields(processor_t *proc, reg_t pc,
                            unsigned xlen, bool interrupt_pending) {
  printf("wfi in debug mode, interrupt_pending=%d\n", interrupt_pending);
  proc->reset();
  auto *state = proc->get_state();
  state->debug_mode = true;
  state->pc = pc;
  if (interrupt_pending) {
    // A pending interrupt cannot be taken in Debug Mode, but it does reach
    // take_interrupt(), which clears in_wfi.
    state->mie->write(MIP_MTIP);
    state->mip->backdoor_write_with_mask(MIP_MTIP, MIP_MTIP);
  }

  proc->step(BUDGET);

  // On RV32 the pc is sign-extended.
  reg_t want = xlen == 32 ? static_cast<int32_t>(pc + 4) : pc + 4;
  bool ok = check_eq(state->pc, want, "pc after one wfi");
  ok &= check_eq(proc->is_waiting_for_interrupt(), false, "in_wfi");
  ok &= check_eq(state->debug_mode, true, "debug mode");
  return ok;
}

static bool run_test(const char *isa, unsigned xlen) {
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

  auto *proc = sim.get_core(size_t(0));
  auto base = cfg.mem_layout.at(0).get_base();

  // Fill the budget's worth of instructions with WFIs, so that a step() which
  // fails to return keeps retiring them instead of trapping.
  std::vector<uint32_t> wfis(BUDGET, WFI);
  if (!mems.at(0).second->store(0, wfis.size() * sizeof(WFI),
                                reinterpret_cast<const uint8_t *>(wfis.data()))) {
    fprintf(stderr, "FAIL: store wfi\n");
    return false;
  }

  bool ok = test_wfi_yields(proc, base, xlen, false);
  ok &= test_wfi_yields(proc, base, xlen, true);
  return ok;
}

int main() {
  bool ok = true;
  for (auto [isa, xlen] : {std::pair{"RV32IMAFDC", 32u}, std::pair{"RV64IMAFDC", 64u}}) {
    printf("%s\n", isa);
    ok &= run_test(isa, xlen);
  }

  if (!ok)
    return EXIT_FAILURE;
  printf("Executed successfully\n");
  return EXIT_SUCCESS;
}
