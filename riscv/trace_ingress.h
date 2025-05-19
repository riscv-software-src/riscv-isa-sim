#ifndef _RISCV_TRACE_INGRESS_H
#define _RISCV_TRACE_INGRESS_H

#include <cstdint>
#include "encoding.h"
#include "decode.h"

enum trace_encoder_type {
  N_TRACE_ENCODER,
  E_TRACE_ENCODER
};

// Enumerates all possible instruction types
enum insn_type {
  I_NONE               = 0,
  I_EXCEPTION          = 1,
  I_INTERRUPT          = 2,
  I_TRAP_RETURN        = 3,
  I_BRANCH_NON_TAKEN   = 4,
  I_BRANCH_TAKEN       = 5,
  // I_BRANCH_UNINFERABLE = 6,
  I_RESERVED           = 7,
  I_CALL_UNINFERABLE   = 8,
  I_CALL_INFERABLE     = 9,
  I_JUMP_UNINFERABLE   = 10,
  I_JUMP_INFERABLE     = 11,
  I_COROUTINE_SWAP     = 12,
  I_RETURN             = 13,
  I_OTHER_UNINFERABLE  = 14,
  I_OTHER_INFERABLE    = 15,
};

// Enumerates all possible privilege level encodings
enum priv_enc {
  P_U  = 0,
  P_S  = 1,
  // 2 is reserved 
  P_M  = 3,
  // 4-7 is optional and unused for now
  P_D  = 4,
  P_VU = 5,
  P_VS = 6,
  // 7 is reserved
};

// Core to encoder communication struct
// Assume a spike hart with max 1 instruction retired per cycle
// Regardless of RV64 or RV32, we fix the size of the instruction address to 64 bits
struct hart_to_encoder_ingress_t {
  // instruction type
  insn_type i_type;       // 4 bits
  // exception cause, mcause
  uint8_t exc_cause;      // 4 bits (only the lower 4 bits are used)
  // trap value, mtval
  uint64_t tval;          // 64 bits
  // core privilege level
  priv_enc priv;          // 3 bits
  // retired instruction address
  uint64_t i_addr;        // 64 bits
  //context, time, ctype, sijump are now unimplemented
  // number of instructions retired - 0 or 1 for spike harts
  bool iretire;           // 1 bit
  // ilastsize
  int ilastsize;          // 2 or 4, 2 bits
  // timestamp
  uint64_t i_timestamp;   // 64 bits
};

#define CHECK_INSN(name)  ((insn->bits() & MASK_##name) == MATCH_##name)

static inline bool _is_branch(insn_t* insn) {
  return CHECK_INSN(BEQ) || CHECK_INSN(BNE) || CHECK_INSN(BLT) || CHECK_INSN(BGE) || CHECK_INSN(BLTU) || CHECK_INSN(BGEU) || CHECK_INSN(C_BEQZ) || CHECK_INSN(C_BNEZ);
}

static inline bool _is_jal(insn_t* insn) {
  return CHECK_INSN(JAL) || CHECK_INSN(C_JAL) || CHECK_INSN(C_J);
}

static inline bool _is_jalr(insn_t* insn) {
  return CHECK_INSN(JALR) || CHECK_INSN(C_JALR) || CHECK_INSN(C_JR);
}

static inline insn_type _get_insn_type(insn_t* insn, bool taken) {
  if (_is_branch(insn)) {
    return taken ? I_BRANCH_TAKEN : I_BRANCH_NON_TAKEN;
  }
  else if (_is_jal(insn)) {
    return I_JUMP_INFERABLE;
  }
  else if (_is_jalr(insn)) {
    return I_JUMP_UNINFERABLE;
  }
  // TODO: further categorization is not implemented for now
  else {
    return I_NONE;
  }
}

#endif // _RISCV_TRACE_INGRESS_H