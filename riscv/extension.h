// See LICENSE for license details.

#ifndef _RISCV_COPROCESSOR_H
#define _RISCV_COPROCESSOR_H

#include "processor.h"
#include "disasm.h"
#include <vector>
#include <functional>

class extension_t
{
 public:
  virtual std::vector<insn_desc_t> get_instructions(const processor_t &proc) = 0;
  virtual std::vector<disasm_insn_t*> get_disasms(const processor_t *proc = nullptr) = 0;
  virtual std::vector<csr_t_p> get_csrs(processor_t &) const { return {}; };
  virtual const char* name() const = 0;
  virtual void reset(processor_t &) {};
  virtual void set_debug(bool UNUSED value, const processor_t &) {}
  virtual ~extension_t() = default;

 protected:
  void illegal_instruction(processor_t &proc);
  void raise_interrupt(processor_t &proc);
  void clear_interrupt(processor_t &proc);
};

std::function<extension_t*()> find_extension(const char* name);
void register_extension(const char* name, std::function<extension_t*()> f);

#define REGISTER_EXTENSION(name, constructor) \
  class register_##name { \
    public: register_##name() { register_extension(#name, constructor); } \
  }; static register_##name dummy_##name;

#endif
