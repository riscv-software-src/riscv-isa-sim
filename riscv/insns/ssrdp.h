#include "zicfiss.h"

if (xSSE()) {
  WRITE_RD(sext_xlen(STATE.ssp->read()));
} else {
  #include "mop_r_N.h"
}
