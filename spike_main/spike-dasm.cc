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
  parser.option(0, "extension", 1, [&](const char* s){extension = find_extension(s);});
  parser.option(0, "isa", 1, [&](const char* s){isa = s;});
  parser.parse(argv);

  processor_t p(isa, 0, 0);
  if (extension)
    p.register_extension(extension());

  while (getline(cin, s))
  {
    for (size_t start = 0; (start = s.find("DASM(", start)) != string::npos; )
    {
      size_t end = s.find(')', start);
      if (end == string::npos)
        break;

      char* endp;
      size_t numstart = start + strlen("DASM(");
      int64_t bits = strtoull(&s[numstart], &endp, 16);
      size_t nbits = 4 * (endp - &s[numstart]);
      if (nbits < 64)
        bits = bits << (64 - nbits) >> (64 - nbits);

      string dis = p.get_disassembler()->disassemble(bits);
      s = s.substr(0, start) + dis + s.substr(end+1);
      start += dis.length();
    }

    cout << s << '\n';
  }

  return 0;
}
