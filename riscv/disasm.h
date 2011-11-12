#ifndef _RISCV_DISASM_H
#define _RISCV_DISASM_H

#include "decode.h"
#include <string>
#include <vector>

struct disasm_insn_t;

class disassembler
{
 public:
  disassembler();
  ~disassembler();
  std::string disassemble(insn_t insn);
 private:
  static const int HASH_SIZE = 256;
  std::vector<const disasm_insn_t*> chain[HASH_SIZE+1];
  void add_insn(disasm_insn_t* insn);
  const disasm_insn_t* lookup(insn_t insn);
};

#endif
