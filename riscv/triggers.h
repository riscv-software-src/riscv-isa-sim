#ifndef _RISCV_TRIGGERS_H
#define _RISCV_TRIGGERS_H

namespace triggers {

typedef enum {
  OPERATION_EXECUTE,
  OPERATION_STORE,
  OPERATION_LOAD,
} operation_t;

};

#endif
