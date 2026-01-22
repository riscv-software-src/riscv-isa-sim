#include "extension.h"
#include "processor.h"

#include <algorithm>

std::vector<insn_func_t> agnostic_postprocesses;

struct agnostic_t : public extension_t {
  const char *name() const override { return "spiketama"; }

  agnostic_t() {}

  std::vector<insn_desc_t> get_instructions(const processor_t &) override {
    std::vector<insn_desc_t> agnostic_insn_list;

#define AGNOSTIC_DECLARE_INSN(name, match, mask)                               \
  insn_bits_t name##_match = (match), name##_mask = (mask);

#include "spiketama_insns.h"
#undef AGNOSTIC_DECLARE_INSN

#define AGNOSTIC_DECLARE_INSN(name, match, mask)                               \
  extern reg_t agnostic_fast_rv32i_##name(processor_t *, insn_t, reg_t);       \
  extern reg_t agnostic_fast_rv64i_##name(processor_t *, insn_t, reg_t);       \
  extern reg_t agnostic_fast_rv32e_##name(processor_t *, insn_t, reg_t);       \
  extern reg_t agnostic_fast_rv64e_##name(processor_t *, insn_t, reg_t);       \
  extern reg_t agnostic_logged_rv32i_##name(processor_t *, insn_t, reg_t);     \
  extern reg_t agnostic_logged_rv64i_##name(processor_t *, insn_t, reg_t);     \
  extern reg_t agnostic_logged_rv32e_##name(processor_t *, insn_t, reg_t);     \
  extern reg_t agnostic_logged_rv64e_##name(processor_t *, insn_t, reg_t);
#include "spiketama_insns.h"
#undef AGNOSTIC_DECLARE_INSN

#define AGNOSTIC_DECLARE_INSN(name, match, mask)                               \
  {                                                                            \
    insn_desc_t insn = {name##_match,                                          \
                        name##_mask,                                           \
                        agnostic_fast_rv32i_##name,                            \
                        agnostic_fast_rv64i_##name,                            \
                        agnostic_fast_rv32e_##name,                            \
                        agnostic_fast_rv64e_##name,                            \
                        agnostic_logged_rv32i_##name,                          \
                        agnostic_logged_rv64i_##name,                          \
                        agnostic_logged_rv32e_##name,                          \
                        agnostic_logged_rv64e_##name};                         \
    agnostic_insn_list.push_back(insn);                                        \
  }
#include "spiketama_insns.h"
#undef AGNOSTIC_DECLARE_INSN

    return agnostic_insn_list;
  }

  std::vector<disasm_insn_t *> get_disasms(const processor_t *) override {
    return {};
  }
};

REGISTER_EXTENSION(spiketama, []() {
  static agnostic_t ext;
  return &ext;
})
