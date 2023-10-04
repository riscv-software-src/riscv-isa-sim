#include "zicfiss.h"

if (xSSE()) {
  POP_VALUE_FROM_SS_AND_CHECK(RS1);
} else {
  #include "mop_r_N.h"
}
