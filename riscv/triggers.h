#ifndef _RISCV_TRIGGERS_H
#define _RISCV_TRIGGERS_H

#include <vector>

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

class matched_t
{
  public:
    matched_t(int index,
        triggers::operation_t operation, reg_t address, reg_t data) :
      index(index), operation(operation), address(address), data(data) {}

    int index;
    triggers::operation_t operation;
    reg_t address;
    reg_t data;
};

class trigger_t {
public:
  bool dmode;
  action_t action;

protected:
  trigger_t() : dmode(false), action(ACTION_DEBUG_EXCEPTION) {};
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

  mcontrol_t();
  reg_t tdata1_read(const processor_t *proc) const noexcept;
  bool tdata1_write(processor_t *proc, const reg_t val) noexcept;
  reg_t tdata2_read(const processor_t *proc) const noexcept;
  bool tdata2_write(processor_t *proc, const reg_t val) noexcept;

  uint8_t type;
  uint8_t maskmax;
  bool select;
  bool timing;
  bool chain;
  match_t match;
  bool m;
  bool h;
  bool s;
  bool u;
  bool execute;
  bool store;
  bool load;
  reg_t tdata2;
};

class module_t {
public:
  module_t(unsigned count);

  // Return the index of a trigger that matched, or -1.
  int trigger_match(triggers::operation_t operation, reg_t address, reg_t data);

  processor_t *proc;
  std::vector<mcontrol_t *> triggers;
};

};

#endif
