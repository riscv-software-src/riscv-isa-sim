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
  disassembler_t d;

  std::function<extension_t*()> extension;
  option_parser_t parser;
  parser.option(0, "extension", 1, [&](const char* s){
    if (!extensions().count(s))
      fprintf(stderr, "unknown extension %s!\n", s), exit(-1);
    extension = extensions()[s];

    for (auto disasm_insn : extension()->get_disasms())
      d.add_insn(disasm_insn);
  });

  while (getline(cin, s))
  {
    for (size_t start = 0; (start = s.find("DASM(", start)) != string::npos; )
    {
      size_t end = s.find(')', start);
      if (end == string::npos)
        break;

      size_t numstart = start + strlen("DASM(");
      uint32_t n = strtoul(&s[numstart], NULL, 16);

      string dis = d.disassemble(*(insn_t*)&n);

      s = s.substr(0, start) + dis + s.substr(end+1);
      start += dis.length();
    }

    cout << s << '\n';
  }

  return 0;
}
