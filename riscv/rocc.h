#ifndef _RISCV_ROCC_H
#define _RISCV_ROCC_H

#include "extension.h"

struct rocc_insn_t
{
  unsigned opcode : 7;
  unsigned rd : 5;
  unsigned xs2 : 1;
  unsigned xs1 : 1;
  unsigned xd : 1;
  unsigned rs1 : 5;
  unsigned rs2 : 5;
  unsigned funct : 7;
};

union rocc_insn_union_t
{
  rocc_insn_t r;
  insn_t i;
};

class rocc_t : public extension_t
{
 public:
  virtual reg_t custom0(rocc_insn_t insn, reg_t xs1, reg_t xs2);
  virtual reg_t custom1(rocc_insn_t insn, reg_t xs1, reg_t xs2);
  virtual reg_t custom2(rocc_insn_t insn, reg_t xs1, reg_t xs2);
  virtual reg_t custom3(rocc_insn_t insn, reg_t xs1, reg_t xs2);
  std::vector<insn_desc_t> get_instructions();
  std::vector<disasm_insn_t*> get_disasms();
};

#define define_custom_func(type_name, ext_name_str, func_name, method_name) \
  static reg_t func_name(processor_t* p, insn_t insn, reg_t pc) \
  { \
    type_name* rocc = static_cast<type_name*>(p->get_extension(ext_name_str)); \
    rocc_insn_union_t u; \
    state_t* state = p->get_state();                          \
    u.i = insn;                                               \
    reg_t xs1 = u.r.xs1 ? state->XPR[insn.rs1()] : -1;        \
    reg_t xs2 = u.r.xs2 ? state->XPR[insn.rs2()] : -1;        \
    reg_t xd = rocc->method_name(u.r, xs1, xs2);              \
    if (u.r.xd) {                                                 \
      state->log_reg_write[insn.rd() << 4] = {xd, 0};             \
      state->XPR.write(insn.rd(), xd);                            \
    }                                                             \
    return pc+4; \
  } \

#define push_custom_insn(insn_list, opcode, opcode_mask, func_name_32, func_name_64) \
  insn_list.push_back((insn_desc_t){opcode, opcode_mask,                \
    func_name_32, func_name_64,                                         \
    func_name_32, func_name_64,                                         \
    func_name_32, func_name_64,                                         \
    func_name_32, func_name_64,                                         \
  })                                                                    \

#define ILLEGAL_INSN_FUNC &::illegal_instruction

#define ROCC_OPCODE0 0x0b
#define ROCC_OPCODE1 0x2b
#define ROCC_OPCODE2 0x5b
#define ROCC_OPCODE3 0x7b

#define ROCC_OPCODE_MASK 0x7f

#endif
