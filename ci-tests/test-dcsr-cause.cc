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

// mstatus.MDT set makes the next M-mode trap a double trap, which Smdbltrp
// turns into a critical error. dcsr.step is set the way dret does it.
static void setup_double_trap_step(state_t *state, reg_t pc) {
  state->pc = pc;
  state->mstatus->write(set_field(state->mstatus->read(), MSTATUS_MDT, 1));
  state->dcsr->write(set_field(state->dcsr->read(), DCSR_STEP, 1));
  state->single_step = state->STEP_STEPPING;
}

int main() {
  cfg_t cfg;
  cfg.isa = "RV64IMAFDC_Smdbltrp";

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
  auto *state = proc->get_state();
  auto base = cfg.mem_layout.at(0).get_base();

  // An all-zero word is an illegal instruction. Store it through the backing
  // mem_t to avoid address translation.
  uint32_t illegal = 0;
  bool ok = check(mems.at(0).second->store(0, sizeof(illegal),
                                           reinterpret_cast<const uint8_t *>(&illegal)),
                  "wrote illegal instruction into DRAM") &&
            check(!state->debug_mode, "precondition: not in debug mode");
  setup_double_trap_step(state, base);

  proc->step(1);

  ok = ok && check(state->debug_mode, "hart entered debug mode") &&
       check_eq(get_field(state->dcsr->read(), DCSR_CAUSE), DCSR_CAUSE_HALT,
                "dcsr.cause, where STEP means the critical error was overwritten") &&
       check_eq(state->dpc->read(), base, "dpc, which must be the faulting PC");

  if (!ok)
    return EXIT_FAILURE;
  printf("Executed successfully\n");
  return EXIT_SUCCESS;
}
