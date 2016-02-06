// See LICENSE for license details.

#include "trap.h"
#include "processor.h"
#include <cstdio>

const char* trap_t::name()
{
  const char* fmt = uint8_t(which) == which ? "trap #%u" : "interrupt #%u";
  sprintf(_name, fmt, uint8_t(which));
  return _name;
}
