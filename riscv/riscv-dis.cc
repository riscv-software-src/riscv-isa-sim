// See LICENSE for license details.

// This little program finds occurrences of strings like
//  DASM(ffabc013)
// in its input, then replaces them with the disassembly
// enclosed hexadecimal number, interpreted as a RISC-V
// instruction.

#include "disasm.h"
#include <iostream>
#include <string>
#include <cstdint>
using namespace std;

int main()
{
  string s;
  disassembler_t d;

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
