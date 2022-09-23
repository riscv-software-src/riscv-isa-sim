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
  virtual std::vector<insn_desc_t> get_instructions() = 0;
  virtual std::vector<disasm_insn_t*> get_disasms() = 0;
  virtual const char* name() = 0;
  virtual void reset() {};
  virtual void set_debug(bool UNUSED value) {}
  virtual ~extension_t();

  void set_processor(processor_t* _p) { p = _p; }
 protected:
  processor_t* p;

  void illegal_instruction();
  void raise_interrupt();
  void clear_interrupt();
};

std::function<extension_t*()> find_extension(const char* name);
void register_extension(const char* name, std::function<extension_t*()> f);

#define REGISTER_EXTENSION(name, constructor) \
  class register_##name { \
    public: register_##name() { register_extension(#name, constructor); } \
  }; static register_##name dummy_##name;

#endif
