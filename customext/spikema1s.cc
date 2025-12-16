#include "agnostic_macros.h"
#include "extension.h"
#include "processor.h"

#include <algorithm>

extern std::vector<insn_func_t> agnostic_postprocesses;

static reg_t mask_agnostic_fill1s(processor_t *p, insn_t insn, reg_t pc) {
  // rvv-spec-1.0: Vector Loads and Stores: Masked vector loads do not update
  // inactive elements in the destination vector register group, unless masked
  // agnostic is specifed (vtype.vma=1). Masked vector stores only update active
  // memory elements.
  if (is_rvv_or_fp_store(insn) || is_rvv_or_fp_load(insn))
    return pc;
  // Belonging to the V extension is checked after loads and stores, because
  // their opcodes do not match the opcodes of V extension.
  if (!is_rvv(insn) || is_rvv_scalar_dest(insn))
    return pc;
  // When vm==1 then instruction unmasked, therefore, the agnostic policy does
  // not apply to any elements.
  if (insn.v_vm())
    return pc;
  // When vma==0 then undisturbed policy is in effect.
  if (p->VU.vma == 0)
    return pc;

  AGNOSTIC_LOOP_BODY({ // mask agnostic - fill 1s
    if (!p->VU.mask_elt(0, i))
      vd = all_ones;
  });
  return pc;
}

struct mask_agnostic_fill1s_t : public extension_t {
  const char *name() const override { return "spikema1s"; }

  mask_agnostic_fill1s_t() {}

  std::vector<insn_desc_t> get_instructions(const processor_t &) override {
    return {};
  }

  std::vector<disasm_insn_t *> get_disasms(const processor_t *) override {
    return {};
  }

  void reset(processor_t &) override {
    auto &insn_postprocesses = agnostic_postprocesses;
    auto mask_func = std::find(insn_postprocesses.begin(),
                               insn_postprocesses.end(), &mask_agnostic_fill1s);
    if (mask_func == insn_postprocesses.end())
      insn_postprocesses.push_back(&mask_agnostic_fill1s);
  }
};

REGISTER_EXTENSION(spikema1s, []() {
  static mask_agnostic_fill1s_t ext;
  return &ext;
})
