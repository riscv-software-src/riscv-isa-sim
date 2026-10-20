#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include <riscv/sim.h>
#include <riscv/processor.h>
#include <riscv/decode.h>
#include <riscv/encoding.h>

static bool check(bool cond, const char *what) {
  if (!cond)
    fprintf(stderr, "FAIL: %s\n", what);
  return cond;
}

static bool check_eq(reg_t got, reg_t want, const char *what) {
  if (got == want)
    return true;
  fprintf(stderr, "FAIL: %s: got %" PRIu64
      ", want %" PRIu64 "\n", what, got, want);
  return false;
}

int main() {
  cfg_t cfg;

  std::vector<device_factory_sargs_t> plugin_devices;
  std::vector<std::string> htif_args{"none"};

  std::vector<std::unique_ptr<mem_t>> owned_mems;
  std::vector<std::pair<reg_t, abstract_mem_t *>> mems;
  for (const auto &m : cfg.mem_layout) {
    owned_mems.push_back(std::make_unique<mem_t>(m.get_size()));
    mems.emplace_back(m.get_base(), owned_mems.back().get());
  }

  // halted = true leaves resethaltreq pending on the hart.
  sim_t sim(&cfg, true, mems, plugin_devices, false, htif_args,
            debug_module_config_t{}, nullptr, true, nullptr, false, nullptr,
            std::nullopt);

  auto *proc = sim.get_core(size_t(0));
  auto *state = proc->get_state();

  bool ok = check(!state->debug_mode, "precondition: not in debug mode");

  // A debugger halts the hart before it retires its first instruction, so
  // resethaltreq and haltreq are pending at the same step().
  proc->halt_request = proc->HR_REGULAR;

  // n = 0 runs the halt dispatch without fetching from the debug ROM.
  proc->step(0);

  ok = ok && check(state->debug_mode, "hart entered debug mode") &&
      check_eq(get_field(state->dcsr->read(), DCSR_CAUSE), DCSR_CAUSE_HALT,
               "dcsr.cause, where DEBUGINT means haltreq outranked resethaltreq");

  // Resume the hart the way dret does, minus the debug ROM. The halt-on-reset
  // request is spent, so the hart must not halt again until the next reset.
  state->debug_mode = false;
  proc->halt_request = proc->HR_NONE;
  proc->step(0);

  ok = ok && check(!state->debug_mode, "hart stayed running after resume");

  if (!ok)
    return EXIT_FAILURE;
  printf("Executed successfully\n");
  return EXIT_SUCCESS;
}
