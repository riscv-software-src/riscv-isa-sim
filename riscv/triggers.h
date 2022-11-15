#ifndef _RISCV_TRIGGERS_H
#define _RISCV_TRIGGERS_H

#include <vector>
#include <optional>

#include "decode.h"

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
  ACTION_TRACE_EMIT = MCONTROL_ACTION_TRACE_EMIT
} action_t;

typedef enum {
  MATCH_NONE,
  MATCH_FIRE_BEFORE,
  MATCH_FIRE_AFTER
} match_result_t;

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
  virtual bool tdata1_write(processor_t * const proc, const reg_t val) noexcept = 0;
  virtual reg_t tdata2_read(const processor_t * const proc) const noexcept = 0;
  virtual bool tdata2_write(processor_t * const proc, const reg_t val) noexcept = 0;

  virtual bool get_dmode() const = 0;
  virtual bool get_chain() const { return false; }
  virtual bool get_execute() const { return false; }
  virtual bool get_store() const { return false; }
  virtual bool get_load() const { return false; }
  virtual action_t get_action() const { return ACTION_DEBUG_EXCEPTION; }

  virtual match_result_t memory_access_match(processor_t * const proc,
      operation_t operation, reg_t address, std::optional<reg_t> data) = 0;
};

class tdata2_csr_t : public virtual trigger_t {
public:
  reg_t tdata2_read(const processor_t * const proc) const noexcept override;
  bool tdata2_write(processor_t * const proc, const reg_t val) noexcept override;

protected:
  reg_t tdata2;
};

class mcontrol_t : public tdata2_csr_t {
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
  virtual bool tdata1_write(processor_t * const proc, const reg_t val) noexcept override;

  virtual bool get_dmode() const override { return dmode; }
  virtual bool get_chain() const override { return chain; }
  virtual bool get_execute() const override { return execute; }
  virtual bool get_store() const override { return store; }
  virtual bool get_load() const override { return load; }
  virtual action_t get_action() const override { return action; }

  virtual match_result_t memory_access_match(processor_t * const proc,
      operation_t operation, reg_t address, std::optional<reg_t> data) override;

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

  unsigned count() const { return triggers.size(); }

  match_result_t memory_access_match(action_t * const action,
      operation_t operation, reg_t address, std::optional<reg_t> data);

  reg_t tdata1_read(const processor_t * const proc, unsigned index) const noexcept;
  bool tdata1_write(processor_t * const proc, unsigned index, const reg_t val) noexcept;
  reg_t tdata2_read(const processor_t * const proc, unsigned index) const noexcept;
  bool tdata2_write(processor_t * const proc, unsigned index, const reg_t val) noexcept;

  processor_t *proc;
private:
  std::vector<trigger_t *> triggers;
};

};

#endif
