#include "trap.h"

const char* trap_name(trap_t t)
{
  #define DECLARE_TRAP(x) "trap_"#x
  static const char* names[] = { TRAP_LIST };
  #undef DECLARE_TRAP

  return (unsigned)t >= sizeof(names)/sizeof(names[0]) ? "unknown" : names[t];
}
