// See LICENSE for license details.

#include "insn_template.h"
#include "insn_macros.h"

#define DECODE_MACRO_USAGE_LOGGED 0

#define PROLOGUE \
  reg_t npc = sext_xlen(pc + insn_length(OPCODE)); \
  icache_entry_t* next_icache_entry = icache_entry + insn_length(OPCODE) / sizeof(insn_parcel_t); \
  if (!p->extension_enabled(EXT_ZCA)) assume(insn_length(OPCODE) % 4 == 0)

#define EPILOGUE \
  trace_opcode(p, OPCODE, insn); \
  return std::make_tuple(npc, next_icache_entry)

std::tuple<reg_t, icache_entry_t*> fast_rv32i_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

std::tuple<reg_t, icache_entry_t*> fast_rv64i_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 1

std::tuple<reg_t, icache_entry_t*> logged_rv32i_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

std::tuple<reg_t, icache_entry_t*> logged_rv64i_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

#undef CHECK_REG
#define CHECK_REG(reg) require((reg) < 16)

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 0

std::tuple<reg_t, icache_entry_t*> fast_rv32e_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

std::tuple<reg_t, icache_entry_t*> fast_rv64e_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

#undef DECODE_MACRO_USAGE_LOGGED
#define DECODE_MACRO_USAGE_LOGGED 1

std::tuple<reg_t, icache_entry_t*> logged_rv32e_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 32
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}

std::tuple<reg_t, icache_entry_t*> logged_rv64e_NAME(processor_t* p, icache_entry_t* icache_entry, insn_t insn, reg_t pc)
{
  #define xlen 64
  PROLOGUE;
  #include "insns/NAME.h"
  EPILOGUE;
  #undef xlen
}
