// See LICENSE for license details.

#ifndef _RISCV_COPROCESSOR_H
#define _RISCV_COPROCESSOR_H

#include "processor.h"
#include "disasm.h"
#include <vector>
#include <functional>

typedef enum {
  EXT_STATE_OFF,
  EXT_STATE_INIT,
  EXT_STATE_CLEAN,
  EXT_STATE_DIRTY,

  MAX_EXT_STATE
} extension_state_t;

class extension_t
{
 public:
  extension_t(bool always_dirty=false) : always_dirty(always_dirty) {}
  virtual std::vector<insn_desc_t> get_instructions() = 0;
  virtual std::vector<disasm_insn_t*> get_disasms() = 0;
  virtual const char* name() = 0;
  virtual void reset() { state = always_dirty ? EXT_STATE_DIRTY : EXT_STATE_OFF; }
  virtual void set_debug(bool value) {};
  virtual ~extension_t();

  void set_processor(processor_t* _p) { p = _p; }
  extension_state_t get_state() const { return state; }
  void set_state(extension_state_t s) { state = s; }
 protected:
  processor_t* p;

  void illegal_instruction();
  void raise_interrupt();
  void clear_interrupt();

 private:
  const bool always_dirty;
  extension_state_t state;
};

std::function<extension_t*()> find_extension(const char* name);
void register_extension(const char* name, std::function<extension_t*()> f);

#define REGISTER_EXTENSION(name, constructor) \
  class register_##name { \
    public: register_##name() { register_extension(#name, constructor); } \
  }; static register_##name dummy_##name;

class xs_gatherer_t
{
public:
  xs_gatherer_t(processor_t* proc) : p(proc) {}
  void reset();

  extension_state_t get_xs();
  void register_extension(extension_t*);
  extension_t* get_extension();
  extension_t* get_extension(const char* name);
  bool any_custom_extensions() const {
    return !custom_extensions.empty();
  }
protected:
  processor_t* p;
  std::unordered_map<std::string, extension_t*> custom_extensions;
};

#endif
