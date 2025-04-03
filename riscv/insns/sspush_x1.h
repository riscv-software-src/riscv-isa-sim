#include "zicfiss.h"

if (xSSE()) {
  PUSH_VALUE_TO_SS(RS2);
} else {
  #include "mop_rr_N.h"
}
