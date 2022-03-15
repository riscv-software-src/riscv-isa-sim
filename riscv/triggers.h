#ifndef _RISCV_TRIGGERS_H
#define _RISCV_TRIGGERS_H

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

class mcontrol_t {
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

  uint8_t type;
  bool dmode;
  uint8_t maskmax;
  bool select;
  bool timing;
  triggers::action_t action;
  bool chain;
  match_t match;
  bool m;
  bool h;
  bool s;
  bool u;
  bool execute;
  bool store;
  bool load;
};

};

#endif
