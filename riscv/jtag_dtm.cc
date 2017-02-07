#include <stdio.h>

#include "jtag_dtm.h"

#if 1
#  define D(x) x
#else
#  define D(x)
#endif

void jtag_dtm_t::set_pins(bool tck, bool tms, bool tdi) {
  if (!_tck && tck) {
    // Positive clock edge.

    switch (state) {
      case SHIFT_DR:
        dr >>= 1;
        dr |= (uint64_t) _tdi << (dr_length-1);
        break;
      case SHIFT_IR:
        ir >>= 1;
        ir |= _tdi << (ir_length-1);
        break;
      default:
        break;
    }
    state = next[state][_tms];
    switch (state) {
      case TEST_LOGIC_RESET:
        ir = idcode_ir;
        break;
      case CAPTURE_DR:
        capture_dr();
        break;
      case SHIFT_DR:
        _tdo = dr & 1;
        break;
      case UPDATE_DR:
        update_dr();
        break;
      case CAPTURE_IR:
        break;
      case SHIFT_IR:
        _tdo = ir & 1;
        break;
      case UPDATE_IR:
        break;
      default:
        break;
    }
  }

  _tck = tck;
  _tms = tms;
  _tdi = tdi;

  D(fprintf(stderr, "state=%2d tck=%d tms=%d tdi=%d tdo=%d ir=0x%x dr=0x%lx\n",
        state, _tck, _tms, _tdi, _tdo, ir, dr));
}

void jtag_dtm_t::capture_dr()
{
  switch (ir) {
    case idcode_ir:
      dr = 0xdeadbeef;
      dr_length = 32;
      break;
    case dtmcontrol_ir:
      dr = dtmcontrol;
      dr_length = 32;
    default:
      D(fprintf(stderr, "Unsupported IR: 0x%x\n", ir));
      break;
  }
  D(fprintf(stderr, "Capture DR; IR=0x%x, DR=0x%lx (%d bits)\n",
        ir, dr, dr_length));
}

void jtag_dtm_t::update_dr()
{
}
