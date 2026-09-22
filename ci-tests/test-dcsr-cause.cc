#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include <riscv/sim.h>
#include <riscv/processor.h>
#include <riscv/decode.h>
#include <riscv/encoding.h>
#include <riscv/debug_rom_defines.h>

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

static void step_illegal(processor_t *proc, reg_t pc, bool double_trap, bool cetrig) {
  proc->reset();
  auto *state = proc->get_state();
  state->pc = pc;
  state->mtvec->write(pc + 0x100);
  state->mstatus->write(set_field(state->mstatus->read(), MSTATUS_MDT, double_trap));
  state->dcsr->write(set_field(state->dcsr->read(), DCSR_CETRIG, cetrig));
  state->dcsr->write(set_field(state->dcsr->read(), DCSR_STEP, 1));
  state->single_step = state->STEP_STEPPING;
  proc->step(1);
}

static bool test_double_trap_step(processor_t *proc, reg_t pc, bool cetrig) {
  printf("double trap during single-step\n");
  step_illegal(proc, pc, true, cetrig);
  auto *state = proc->get_state();

  bool ok = check(state->debug_mode, "debug mode");
  ok &= check(state->critical_error, "critical error");
  ok &= check_eq(get_field(state->dcsr->read(), DCSR_CAUSE),
                 cetrig ? DCSR_CAUSE_EXTCAUSE : DCSR_CAUSE_HALT, "dcsr.cause");
  ok &= check_eq(get_field(state->dcsr->read(), DCSR_EXTCAUSE),
                 DCSR_EXTCAUSE_CRITERR, "dcsr.extcause");
  ok &= check_eq(state->dpc->read(), pc, "dpc");
  ok &= check_eq(state->pc, DEBUG_ROM_ENTRY, "pc");
  ok &= check_eq(state->single_step, state->STEP_NONE, "single_step");
  return ok;
}

static bool test_debug_trap(processor_t *proc, reg_t pc, bool cetrig) {
  printf("trap in debug mode after critical error\n");
  step_illegal(proc, pc, true, cetrig);
  auto *state = proc->get_state();
  if (!check(state->debug_mode && state->critical_error, "critical error debug entry"))
    return false;

  auto dcsr = state->dcsr->read();
  auto dpc = state->dpc->read();
  state->pc = pc;
  proc->step(1);

  bool ok = check_eq(state->pc, DEBUG_ROM_TVEC, "pc");
  ok &= check_eq(state->dpc->read(), dpc, "dpc");
  ok &= check_eq(state->dcsr->read(), dcsr, "dcsr");
  return ok;
}

static bool test_trap_step(processor_t *proc, reg_t pc, bool cetrig) {
  printf("ordinary trap during single-step\n");
  step_illegal(proc, pc, false, cetrig);
  auto *state = proc->get_state();

  bool ok = check(state->debug_mode, "debug mode");
  ok &= check(!state->critical_error, "no critical error");
  ok &= check_eq(state->mcause->read(), CAUSE_ILLEGAL_INSTRUCTION, "mcause");
  ok &= check_eq(state->mepc->read(), pc, "mepc");
  ok &= check_eq(state->dpc->read(), pc + 0x100, "dpc");
  ok &= check_eq(get_field(state->dcsr->read(), DCSR_CAUSE), DCSR_CAUSE_STEP, "dcsr.cause");
  ok &= check_eq(state->single_step, state->STEP_NONE, "single_step");
  return ok;
}

static bool run_test(const char *isa, bool cetrig) {
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

  // An all-zero word is an illegal instruction. Store it through the backing
  // mem_t to avoid address translation.
  uint32_t illegal = 0;
  if (!check(mems.at(0).second->store(0, sizeof(illegal),
                                    reinterpret_cast<const uint8_t *>(&illegal)),
             "store illegal instruction"))
    return false;

  bool ok = test_double_trap_step(proc, base, cetrig);
  ok &= test_debug_trap(proc, base, cetrig);
  ok &= test_trap_step(proc, base, cetrig);
  return ok;
}

int main() {
  bool ok = true;
  for (const char *isa : {"RV32IMAFDC_Smdbltrp", "RV64IMAFDC_Smdbltrp"}) {
    for (bool cetrig : {false, true}) {
      printf("%s, cetrig=%d\n", isa, cetrig);
      ok &= run_test(isa, cetrig);
    }
  }

  if (!ok)
    return EXIT_FAILURE;
  printf("Executed successfully\n");
  return EXIT_SUCCESS;
}
