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
  virtual match_result_t memory_access_match(processor_t * const proc,
      operation_t operation, reg_t address, std::optional<reg_t> data) = 0;

  virtual reg_t tdata1_read(const processor_t * const proc) const noexcept = 0;
  virtual bool tdata1_write(processor_t * const proc, const reg_t val) noexcept = 0;
  virtual reg_t tdata2_read(const processor_t * const proc) const noexcept = 0;
  virtual bool tdata2_write(processor_t * const proc, const reg_t val) noexcept = 0;

  virtual bool chain() const { return false; }
  virtual bool execute() const { return false; }
  virtual bool store() const { return false; }
  virtual bool load() const { return false; }

  bool dmode = false;
  action_t action = ACTION_DEBUG_EXCEPTION;
  bool hit = false;

  virtual ~trigger_t() {};

protected:
  trigger_t() {}
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
  virtual bool tdata1_write(processor_t * const proc, const reg_t val) noexcept override;
  virtual reg_t tdata2_read(const processor_t * const proc) const noexcept override;
  virtual bool tdata2_write(processor_t * const proc, const reg_t val) noexcept override;

  virtual bool chain() const override { return chain_bit; }
  virtual bool execute() const override { return execute_bit; }
  virtual bool store() const override { return store_bit; }
  virtual bool load() const override { return load_bit; }

  virtual match_result_t memory_access_match(processor_t * const proc,
      operation_t operation, reg_t address, std::optional<reg_t> data) override;

private:
  bool simple_match(unsigned xlen, reg_t value) const;

public:
  bool select = false;
  bool timing = false;
  bool chain_bit = false;
  match_t match = MATCH_EQUAL;
  bool m = false;
  bool s = false;
  bool u = false;
  bool execute_bit = false;
  bool store_bit = false;
  bool load_bit = false;
  reg_t tdata2;
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
