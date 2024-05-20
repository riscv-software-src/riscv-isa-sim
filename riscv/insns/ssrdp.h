#include "zicfiss.h"

if (xSSE()) {
  WRITE_RD(STATE.ssp->read());
} else {
  #include "mop_r_N.h"
}
