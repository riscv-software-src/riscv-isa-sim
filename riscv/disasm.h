// See LICENSE for license details.

#ifndef _RISCV_DISASM_H
#define _RISCV_DISASM_H

#include "decode.h"
#include <string>
#include <sstream>
#include <vector>

static const char* xpr_name[] = {
  "zero", "ra", "s0", "s1",  "s2",  "s3",  "s4",  "s5",
  "s6",   "s7", "s8", "s9", "s10", "s11",  "sp",  "tp",
  "v0",   "v1", "a0", "a1",  "a2",  "a3",  "a4",  "a5",
  "a6",   "a7", "t0", "t1",  "t2",  "t3",  "t4",  "gp"
};

static const char* fpr_name[] = {
  "fs0", "fs1",  "fs2",  "fs3",  "fs4",  "fs5",  "fs6",  "fs7",
  "fs8", "fs9", "fs10", "fs11", "fs12", "fs13", "fs14", "fs15",
  "fv0", "fv1", "fa0",   "fa1",  "fa2",  "fa3",  "fa4",  "fa5",
  "fa6", "fa7", "ft0",   "ft1",  "ft2",  "ft3",  "ft4",  "ft5"
};

class arg_t
{
 public:
  virtual std::string to_string(insn_t val) const = 0;
  virtual ~arg_t() {}
};

class disasm_insn_t
{
 public:
  disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                const std::vector<const arg_t*>& args)
    : match(match), mask(mask), args(args), name(name) {}

  bool operator == (insn_t insn) const
  {
    return (insn.bits() & mask) == match;
  }

  std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    int len;
    for (len = 0; name[len]; len++)
      s << (name[len] == '_' ? '.' : name[len]);

    if (args.size())
    {
      s << std::string(std::max(1, 8 - len), ' ');
      for (size_t i = 0; i < args.size()-1; i++)
        s << args[i]->to_string(insn) << ", ";
      s << args[args.size()-1]->to_string(insn);
    }
    return s.str();
  }

  uint32_t get_match() const { return match; }
  uint32_t get_mask() const { return mask; }

 private:
  uint32_t match;
  uint32_t mask;
  std::vector<const arg_t*> args;
  const char* name;
};

class disassembler_t
{
 public:
  disassembler_t();
  ~disassembler_t();
  std::string disassemble(insn_t insn);
  void add_insn(disasm_insn_t* insn);
 private:
  static const int HASH_SIZE = 256;
  std::vector<const disasm_insn_t*> chain[HASH_SIZE+1];
  const disasm_insn_t* lookup(insn_t insn);
};

#endif
