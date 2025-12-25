#include "insn_macros.h"
#include "insn_template.h"

extern std::vector<insn_func_t> agnostic_postprocesses;

#define DECODE_MACRO_USAGE_LOGGED 0

#define RVV_INSN_LENGTH 4
#define AGNOSTIC_PROLOGUE reg_t npc = sext_xlen(pc + RVV_INSN_LENGTH)

#define AGNOSTIC_EPILOGUE                                                      \
  for (auto postproc : agnostic_postprocesses)                                 \
    postproc(p, insn, pc);                                                     \
  return npc

reg_t agnostic_fast_rv32i_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 32
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

reg_t agnostic_fast_rv64i_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 64
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 1

reg_t agnostic_logged_rv32i_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 32
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

reg_t agnostic_logged_rv64i_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 64
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

#undef CHECK_REG
#define CHECK_REG(reg) require((reg) < 16)

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 0

reg_t agnostic_fast_rv32e_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 32
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

reg_t agnostic_fast_rv64e_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 64
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 1

reg_t agnostic_logged_rv32e_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 32
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

reg_t agnostic_logged_rv64e_NAME(processor_t *p, insn_t insn, reg_t pc) {
#define xlen 64
  AGNOSTIC_PROLOGUE;
#include "../riscv/insns/NAME.h"
  AGNOSTIC_EPILOGUE;
#undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
