#ifndef _RISCV_TRIGGERS_H
#define _RISCV_TRIGGERS_H

#include <vector>
#include <optional>

#include "encoding.h"

namespace triggers {

typedef enum {
  OPERATION_EXECUTE,
  OPERATION_STORE,
  OPERATION_LOAD,
} operation_t;

typedef enum
{
  ACTION_DEBUG_EXCEPTION = MCONTROL_ACTION_DEBUG_EXCEPTION,
  ACTION_DEBUG_MODE = MCONTROL_ACTION_DEBUG_MODE,
  ACTION_TRACE_START = MCONTROL_ACTION_TRACE_START,
  ACTION_TRACE_STOP = MCONTROL_ACTION_TRACE_STOP,
  ACTION_TRACE_EMIT = MCONTROL_ACTION_TRACE_EMIT,
  ACTION_MAXVAL = MCONTROL_ACTION_TRACE_EMIT
} action_t;

typedef enum {
  TIMING_BEFORE = 0,
  TIMING_AFTER = 1
} timing_t;

struct match_result_t {
  match_result_t(const timing_t t=TIMING_BEFORE, const action_t a=ACTION_DEBUG_EXCEPTION) {
    timing = t;
    action = a;
  }
  timing_t timing;
  action_t action;
};

class matched_t
{
  public:
    matched_t(triggers::operation_t operation, reg_t address, action_t action) :
      operation(operation), address(address), action(action) {}

    triggers::operation_t operation;
    reg_t address;
    action_t action;
};

class trigger_t {
public:
  virtual ~trigger_t() {};

  virtual reg_t tdata1_read(const processor_t * const proc) const noexcept = 0;
  virtual void tdata1_write(processor_t * const proc, const reg_t val, const bool allow_chain) noexcept = 0;
  reg_t tdata2_read(const processor_t * const proc) const noexcept;
  void tdata2_write(processor_t * const proc, const reg_t val) noexcept;

  virtual bool get_dmode() const = 0;
  virtual bool get_chain() const { return false; }
  virtual bool get_execute() const { return false; }
  virtual bool get_store() const { return false; }
  virtual bool get_load() const { return false; }
  virtual action_t get_action() const { return ACTION_DEBUG_EXCEPTION; }

  virtual std::optional<match_result_t> detect_memory_access_match(processor_t UNUSED * const proc,
      operation_t UNUSED operation, reg_t UNUSED address, std::optional<reg_t> UNUSED data) noexcept { return std::nullopt; }
  virtual std::optional<match_result_t> detect_trap_match(processor_t UNUSED * const proc, const trap_t UNUSED & t) noexcept { return std::nullopt; }

protected:
  action_t legalize_action(reg_t val) const noexcept;
  reg_t tdata2;
};

class disabled_trigger_t : public trigger_t {
public:
  virtual reg_t tdata1_read(const processor_t * const proc) const noexcept override;
  virtual void tdata1_write(processor_t * const proc, const reg_t val, const bool allow_chain) noexcept override;

  virtual bool get_dmode() const override { return dmode; }

private:
  bool dmode;
};

class itrigger_t : public trigger_t {
public:
  virtual reg_t tdata1_read(const processor_t * const proc) const noexcept override;
  virtual void tdata1_write(processor_t * const proc, const reg_t val, const bool allow_chain) noexcept override;

  bool get_dmode() const override { return dmode; }
  virtual action_t get_action() const override { return action; }

  virtual std::optional<match_result_t> detect_trap_match(processor_t * const proc, const trap_t& t) noexcept override;

private:
  bool dmode;
  bool hit;
  bool vs;
  bool vu;
  bool nmi;
  bool m;
  bool s;
  bool u;
  action_t action;
};

class etrigger_t : public trigger_t {
public:
  virtual reg_t tdata1_read(const processor_t * const proc) const noexcept override;
  virtual void tdata1_write(processor_t * const proc, const reg_t val, const bool allow_chain) noexcept override;

  bool get_dmode() const override { return dmode; }
  virtual action_t get_action() const override { return action; }

  virtual std::optional<match_result_t> detect_trap_match(processor_t * const proc, const trap_t& t) noexcept override;

private:
  bool dmode;
  bool hit;
  bool vs;
  bool vu;
  bool m;
  bool s;
  bool u;
  action_t action;
};

class mcontrol_t : public trigger_t {
public:
  typedef enum
  {
    MATCH_EQUAL = MCONTROL_MATCH_EQUAL,
    MATCH_NAPOT = MCONTROL_MATCH_NAPOT,
    MATCH_GE = MCONTROL_MATCH_GE,
    MATCH_LT = MCONTROL_MATCH_LT,
    MATCH_MASK_LOW = MCONTROL_MATCH_MASK_LOW,
    MATCH_MASK_HIGH = MCONTROL_MATCH_MASK_HIGH
  } match_t;

  virtual reg_t tdata1_read(const processor_t * const proc) const noexcept override;
  virtual void tdata1_write(processor_t * const proc, const reg_t val, const bool allow_chain) noexcept override;

  virtual bool get_dmode() const override { return dmode; }
  virtual bool get_chain() const override { return chain; }
  virtual bool get_execute() const override { return execute; }
  virtual bool get_store() const override { return store; }
  virtual bool get_load() const override { return load; }
  virtual action_t get_action() const override { return action; }

  virtual std::optional<match_result_t> detect_memory_access_match(processor_t * const proc,
      operation_t operation, reg_t address, std::optional<reg_t> data) noexcept override;

private:
  bool simple_match(unsigned xlen, reg_t value) const;

  bool dmode = false;
  action_t action = ACTION_DEBUG_EXCEPTION;
  bool hit = false;
  bool select = false;
  bool timing = false;
  bool chain = false;
  match_t match = MATCH_EQUAL;
  bool m = false;
  bool s = false;
  bool u = false;
  bool execute = false;
  bool store = false;
  bool load = false;
};

class module_t {
public:
  module_t(unsigned count);
  ~module_t();

  reg_t tdata1_read(const processor_t * const proc, unsigned index) const noexcept;
  bool tdata1_write(processor_t * const proc, unsigned index, const reg_t val) noexcept;
  reg_t tdata2_read(const processor_t * const proc, unsigned index) const noexcept;
  bool tdata2_write(processor_t * const proc, unsigned index, const reg_t val) noexcept;
  reg_t tinfo_read(const processor_t * const proc, unsigned index) const noexcept;

  unsigned count() const { return triggers.size(); }

  std::optional<match_result_t> detect_memory_access_match(operation_t operation, reg_t address, std::optional<reg_t> data) noexcept;
  std::optional<match_result_t> detect_trap_match(const trap_t& t) noexcept;

  processor_t *proc;
private:
  std::vector<trigger_t *> triggers;
};

};

#endif
