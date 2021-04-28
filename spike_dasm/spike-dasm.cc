// See LICENSE for license details.

// This little program finds occurrences of strings like
//  DASM(ffabc013)
// in its input, then replaces them with the disassembly
// enclosed hexadecimal number, interpreted as a RISC-V
// instruction.

#include "disasm.h"
#include "extension.h"
#include <iostream>
#include <string>
#include <cstdint>
#include <fesvr/option_parser.h>
using namespace std;

int main(int argc, char** argv)
{
  string s;
  const char* isa = DEFAULT_ISA;

  std::function<extension_t*()> extension;
  option_parser_t parser;
#ifdef HAVE_DLOPEN
  parser.option(0, "extension", 1, [&](const char* s){extension = find_extension(s);});
#endif
  parser.option(0, "isa", 1, [&](const char* s){isa = s;});
  parser.parse(argv);

  std::string lowercase;
  for (const char *p = isa; *p; p++)
    lowercase += std::tolower(*p);

  int xlen;
  if (lowercase.compare(0, 4, "rv32") == 0) {
    xlen = 32;
  } else if (lowercase.compare(0, 4, "rv64") == 0) {
    xlen = 64;
  } else {
    fprintf(stderr, "bad ISA string: %s\n", isa);
    return 1;
  }

  disassembler_t* disassembler = new disassembler_t(xlen);
  if (extension) {
    for (auto disasm_insn : extension()->get_disasms()) {
      disassembler->add_insn(disasm_insn);
    }
  }

  while (getline(cin, s))
  {
    for (size_t pos = 0; (pos = s.find("DASM(", pos)) != string::npos; )
    {
      size_t start = pos;

      pos += strlen("DASM(");

      if (s[pos] == '0' && (s[pos+1] == 'x' || s[pos+1] == 'X'))
        pos += 2;

      if (!isxdigit(s[pos]))
        continue;

      char* endp;
      int64_t bits = strtoull(&s[pos], &endp, 16);
      if (*endp != ')')
        continue;

      size_t nbits = 4 * (endp - &s[pos]);
      if (nbits < 64)
        bits = bits << (64 - nbits) >> (64 - nbits);

      string dis = disassembler->disassemble(bits);
      s = s.substr(0, start) + dis + s.substr(endp - &s[0] + 1);
      pos = start + dis.length();
    }

    cout << s << '\n';
  }

  return 0;
}
