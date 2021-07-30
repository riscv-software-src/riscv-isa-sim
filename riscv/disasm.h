// See LICENSE for license details.

#ifndef _RISCV_DISASM_H
#define _RISCV_DISASM_H

#include "decode.h"
#include <string>
#include <sstream>
#include <vector>

extern const char* xpr_name[NXPR];
extern const char* fpr_name[NFPR];
extern const char* vr_name[NVPR];
extern const char* csr_name(int which);

class arg_t
{
 public:
  virtual std::string to_string(insn_t val) const = 0;
  virtual ~arg_t() {}
};

class disasm_insn_t
{
 public:
  NOINLINE disasm_insn_t(const char* name, uint32_t match, uint32_t mask,
                         const std::vector<const arg_t*>& args)
    : match(match), mask(mask), args(args), name(strdup(name)) {}
  ~disasm_insn_t() { free(const_cast<char *>(name)); }

  bool operator == (insn_t insn) const
  {
    return (insn.bits() & mask) == match;
  }

  const char* get_name() const
  {
    return name;
  }

  std::string to_string(insn_t insn) const
  {
    std::stringstream s;
    int len;
    for (len = 0; name[len]; len++)
      s << (name[len] == '_' ? '.' : name[len]);

    if (args.size())
    {
      bool next_arg_optional  = false;
      s << std::string(std::max(1, 8 - len), ' ');
      for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == nullptr) {
          next_arg_optional = true;
          continue;
        }
        std::string argString = args[i]->to_string(insn);
        if (next_arg_optional) {
          next_arg_optional = false;
          if (argString.empty()) continue;
        }
        if (i != 0) s << ", ";
        s << argString;
      }
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
  disassembler_t(int xlen);
  ~disassembler_t();

  std::string disassemble(insn_t insn) const;
  const disasm_insn_t* lookup(insn_t insn) const;

  void add_insn(disasm_insn_t* insn);

 private:
  static const int HASH_SIZE = 256;
  std::vector<const disasm_insn_t*> chain[HASH_SIZE+1];
};

#endif
