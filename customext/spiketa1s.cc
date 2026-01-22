#include "agnostic_macros.h"
#include "extension.h"
#include "processor.h"

extern std::vector<insn_func_t> agnostic_postprocesses;

static reg_t tail_agnostic_fill1s(processor_t *p, insn_t insn, reg_t pc) {
  // TODO: So far, only instruction vadd.vv is supported.
  if (!is_vadd_vv(insn))
    return pc;
  // When vta==0 then undisturbed policy is in effect.
  if (p->VU.vta == 0)
    return pc;
  reg_t vl = p->VU.vl->read();
  // rvv-spec-1.0: When vstart ≥ vl, there are no body elements, and no
  // elements are updated in any destination vector register group, including
  // that no tail elements are updated with agnostic values. As a consequence,
  // when vl=0, no elements, including agnostic elements, are updated in the
  // destination vector register group regardless of vstart.
  // FIXME: Vstart is used here, which was reset to 0 as a result of executing
  // this instruction, we need to get the old value of Vstart here.
  if (p->VU.vstart->read() >= vl || vl == 0)
    return pc;

  reg_t tail_from = vl;
  reg_t vl_one_reg = p->VU.VLEN / p->VU.vsew;
  reg_t tail_to = std::max(p->VU.vlmax, vl_one_reg);

  AGNOSTIC_LOOP_TAIL(
      // tail agnostic - fill 1s
      { vd = all_ones; }, tail_from, tail_to);
  return pc;
}

struct tail_agnostic_fill1s_t : public extension_t {
  const char *name() const override { return "spiketa1s"; }

  tail_agnostic_fill1s_t() {}

  std::vector<insn_desc_t> get_instructions(const processor_t &) override {
    return {};
  }

  std::vector<disasm_insn_t *> get_disasms(const processor_t *) override {
    return {};
  }

  void reset(processor_t &) override {
    auto &insn_postprocesses = agnostic_postprocesses;
    auto tail_func = std::find(insn_postprocesses.begin(),
                               insn_postprocesses.end(), &tail_agnostic_fill1s);
    if (tail_func == insn_postprocesses.end())
      insn_postprocesses.push_back(&tail_agnostic_fill1s);
  }
};

REGISTER_EXTENSION(spiketa1s, []() {
  static tail_agnostic_fill1s_t ext;
  return &ext;
})
