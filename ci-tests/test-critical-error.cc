// See LICENSE for license details.

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#include <riscv/sim.h>
#include <riscv/processor.h>
#include <riscv/encoding.h>
#include <riscv/debug_defines.h>
#include <riscv/debug_rom_defines.h>

// Keep the platform alive to check that the errored hart stays stopped, and
// that an external debugger can inspect it before resetting it.
class test_sim_t : public sim_t {
public:
  using sim_t::sim_t;
  void proc_critical_error(unsigned id) override { signals.push_back(id); }
  std::vector<unsigned> signals;
};

static bool check(bool condition, const char *message) {
  if (!condition)
    fprintf(stderr, "FAIL: %s\n", message);
  return condition;
}

static void prepare(processor_t *proc, reg_t pc, bool cetrig) {
  proc->reset();
  auto *s = proc->get_state();
  s->pc = pc;
  s->dcsr->write(set_field(s->dcsr->read(), DCSR_CETRIG, cetrig));
  s->dpc->write(pc + 0x200);
  s->mepc->write(pc + 0x300);
  s->mcause->write(7);
  s->mtval->write(0x1234);
}

static bool test_entry(test_sim_t &sim, reg_t pc, bool cetrig,
                       bool single_step, bool trigger) {
  auto *proc = sim.get_core(size_t(0));
  prepare(proc, pc, cetrig);
  auto *s = proc->get_state();
  if (single_step) {
    s->dcsr->write(s->dcsr->read() | DCSR_STEP);
    s->single_step = s->STEP_STEPPING;
  }
  if (trigger) {
    // With only M-mode, tcontrol.MTE permits action=0 with mstatus.MDT=1.
    proc->put_csr(CSR_TCONTROL, CSR_TCONTROL_MTE);
    proc->put_csr(CSR_TDATA2, pc);
    proc->put_csr(CSR_TDATA1, set_field(reg_t(0), MCONTROL_TYPE(proc->get_xlen()), 6)
                            | MCONTROL_M | MCONTROL_EXECUTE);
  }
  const auto dcsr = s->dcsr->read(), dpc = s->dpc->read();
  const auto mepc = s->mepc->read(), mcause = s->mcause->read();
  const auto mtval = s->mtval->read(), mstatus = s->mstatus->read();
  const auto signals = sim.signals.size();
  proc->step(8);

  bool ok = check(s->critical_error, "critical error is latched");
  ok &= check(s->debug_mode == cetrig, "only cetrig=1 enters Debug Mode");
  ok &= check(sim.signals.size() == signals + !cetrig, "platform notified only for cetrig=0");
  ok &= check(s->mepc->read() == mepc && s->mcause->read() == mcause &&
              s->mtval->read() == mtval && s->mstatus->read() == mstatus,
              "critical error preserves trap CSRs");
  if (cetrig) {
    ok &= check(get_field(s->dcsr->read(), DCSR_CAUSE) == DCSR_CAUSE_EXTCAUSE &&
                get_field(s->dcsr->read(), DCSR_EXTCAUSE) == DCSR_EXTCAUSE_CRITERR,
                "critical-error debug cause has priority over single-step");
    ok &= check(s->pc == DEBUG_ROM_ENTRY && s->dpc->read() == pc, "debug entry PC");
  } else {
    ok &= check(s->dcsr->read() == dcsr && s->dpc->read() == dpc,
                "platform signal preserves debug CSRs");
    ok &= check(s->pc == pc, "critical error preserves PC");
    const auto cycle = s->mcycle->read(), instret = s->minstret->read();
    proc->step(100);
    ok &= check(s->pc == pc && !s->debug_mode && s->mcycle->read() == cycle &&
                s->minstret->read() == instret, "subsequent step cannot execute the errored hart");
    ok &= check(sim.signals.size() == signals + 1 && sim.signals.back() == proc->get_id(),
                "one signal carries the hart ID");
  }
  return ok;
}

static bool test_resume(test_sim_t &sim, reg_t pc, bool cetrig) {
  auto *proc = sim.get_core(size_t(0));
  // Enter Debug Mode from a real critical error, then change cetrig as a debugger.
  prepare(proc, pc, true);
  proc->step(1);
  auto *s = proc->get_state();
  if (!check(s->debug_mode && s->critical_error, "critical-error debug entry before DRET"))
    return false;
  s->dcsr->write(set_field(s->dcsr->read(), DCSR_CETRIG, cetrig));
  s->dpc->write(pc + 0x100);
  s->pc = pc + 4; // DRET in backing memory.
  const auto signals = sim.signals.size();
  proc->step(8);
  bool ok = check(s->critical_error, "DRET retains critical error");
  ok &= check(s->debug_mode == cetrig, "DRET follows updated cetrig");
  ok &= check(s->XPR[5] == 0, "DRET must not execute the resumed instruction");
  ok &= check(sim.signals.size() == signals + !cetrig, "DRET platform notification");
  if (cetrig)
    ok &= check(s->pc == DEBUG_ROM_ENTRY &&
                get_field(s->dcsr->read(), DCSR_CAUSE) == DCSR_CAUSE_EXTCAUSE,
                "DRET immediately re-enters critical-error Debug Mode");
  else
    ok &= check(s->pc == pc + 0x100, "DRET stops at the resume PC");
  return ok;
}

static bool run_test(const char *isa) {
  cfg_t cfg;
  cfg.isa = isa;
  cfg.priv = "M";
  cfg.mem_layout = {mem_cfg_t(0x10000, 0x10000)};
  cfg.hartids = {7};
  cfg.explicit_hartids = true;
  std::vector<device_factory_sargs_t> plugins;
  std::vector<std::string> args{"none"};
  mem_t memory(0x10000);
  const reg_t pc = cfg.mem_layout[0].get_base();
  std::vector<std::pair<reg_t, abstract_mem_t *>> mems{{pc, &memory}};
  // Illegal instruction, DRET, and a register-changing instruction at the
  // resume address. Executing beyond DRET must be detectable without a trap.
  uint32_t instructions[] = {0, 0x7b200073};
  uint32_t increment = 0x00128293; // addi t0, t0, 1
  if (!memory.store(0, sizeof instructions, reinterpret_cast<uint8_t *>(instructions)) ||
      !memory.store(0x100, sizeof increment, reinterpret_cast<uint8_t *>(&increment)))
    return false;
  test_sim_t sim(&cfg, false, mems, plugins, false, args, debug_module_config_t{},
                 nullptr, true, nullptr, false, nullptr, std::nullopt);
  bool ok = true;
  for (bool cetrig : {false, true}) {
    for (bool single_step : {false, true})
      for (bool trigger : {false, true}) {
        printf("%s cetrig=%d step=%d trigger=%d\n", isa, cetrig, single_step, trigger);
        ok &= test_entry(sim, pc + (trigger ? 0x100 : 0), cetrig, single_step, trigger);
      }
    ok &= test_resume(sim, pc, cetrig);
  }
  auto *proc = sim.get_core(size_t(0));
  prepare(proc, pc + 0x100, false);
  proc->step(1);
  ok &= check(!proc->get_state()->critical_error && proc->get_state()->XPR[5] == 1,
              "reset releases the hart to execute normally");

  // The standard Spike platform treats the signal as a failed simulation.
  sim_t platform(&cfg, false, mems, plugins, false, args, debug_module_config_t{},
                  nullptr, true, nullptr, false, nullptr, std::nullopt);
  prepare(platform.get_core(size_t(0)), pc, false);
  platform.get_core(size_t(0))->step(1);
  ok &= check(platform.exit_code() == EXIT_FAILURE,
              "default platform reports a failed simulation");
  return ok;
}

int main() {
  bool ok = true;
  for (const char *isa : {"RV32IMAFDC_Smdbltrp", "RV64IMAFDC_Smdbltrp"})
    ok &= run_test(isa);
  if (!ok)
    return EXIT_FAILURE;
  puts("Executed successfully");
  return EXIT_SUCCESS;
}
