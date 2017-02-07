#include <stdio.h>

#include "decode.h"
#include "jtag_dtm.h"
#include "debug_module.h"

#if 1
#  define D(x) x
#else
#  define D(x)
#endif

enum {
  IR_IDCODE=1,
  IR_DTMCONTROL=0x10,
  IR_DBUS=0x11
};

#define DTMCONTROL_VERSION      0xf
#define DTMCONTROL_ABITS        (0x3f << 4)
#define DTMCONTROL_DBUSSTAT     (3<<10)
#define DTMCONTROL_IDLE         (7<<12)
#define DTMCONTROL_DBUSRESET    (1<<16)

#define DBUS_OP                 3
#define DBUS_DATA               (0xffffffffL<<2)
#define DBUS_ADDRESS            ((1L<<(abits+34)) - (1L<<34))

#define DBUS_OP_STATUS_SUCCESS	0
#define DBUS_OP_STATUS_RESERVED	1
#define DBUS_OP_STATUS_FAILED	2
#define DBUS_OP_STATUS_BUSY	3

#define DBUS_OP_NOP	        0
#define DBUS_OP_READ	        1
#define DBUS_OP_READ_WRITE	2
#define DBUS_OP_RESERVED	3

jtag_dtm_t::jtag_dtm_t(debug_module_t *dm) :
  dm(dm),
  dtmcontrol((abits << 4) | 1),
  dbus(0),
  state(TEST_LOGIC_RESET)
{
}

void jtag_dtm_t::reset() {
  state = TEST_LOGIC_RESET;
}

void jtag_dtm_t::set_pins(bool tck, bool tms, bool tdi) {
  const jtag_state_t next[16][2] = {
    /* TEST_LOGIC_RESET */    { RUN_TEST_IDLE, TEST_LOGIC_RESET },
    /* RUN_TEST_IDLE */       { RUN_TEST_IDLE, SELECT_DR_SCAN },
    /* SELECT_DR_SCAN */      { CAPTURE_DR, SELECT_IR_SCAN },
    /* CAPTURE_DR */          { SHIFT_DR, EXIT1_DR },
    /* SHIFT_DR */            { SHIFT_DR, EXIT1_DR },
    /* EXIT1_DR */            { PAUSE_DR, UPDATE_DR },
    /* PAUSE_DR */            { PAUSE_DR, EXIT2_DR },
    /* EXIT2_DR */            { SHIFT_DR, UPDATE_DR },
    /* UPDATE_DR */           { RUN_TEST_IDLE, SELECT_DR_SCAN },
    /* SELECT_IR_SCAN */      { CAPTURE_IR, TEST_LOGIC_RESET },
    /* CAPTURE_IR */          { SHIFT_IR, EXIT1_IR },
    /* SHIFT_IR */            { SHIFT_IR, EXIT1_IR },
    /* EXIT1_IR */            { PAUSE_IR, UPDATE_IR },
    /* PAUSE_IR */            { PAUSE_IR, EXIT2_IR },
    /* EXIT2_IR */            { SHIFT_IR, UPDATE_IR },
    /* UPDATE_IR */           { RUN_TEST_IDLE, SELECT_DR_SCAN }
  };

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
        ir = IR_IDCODE;
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

  D(fprintf(stderr, "state=%2d, tdi=%d, tdo=%d, tms=%d, tck=%d, ir=0x%02x, dr=0x%lx\n",
        state, _tdi, _tdo, _tms, _tck, ir, dr));

  _tck = tck;
  _tms = tms;
  _tdi = tdi;
}

void jtag_dtm_t::capture_dr()
{
  switch (ir) {
    case IR_IDCODE:
      dr = idcode;
      dr_length = 32;
      break;
    case IR_DTMCONTROL:
      dr = dtmcontrol;
      dr_length = 32;
      break;
    case IR_DBUS:
      dr = dbus;
      dr_length = abits + 34;
      break;
    default:
      D(fprintf(stderr, "Unsupported IR: 0x%x\n", ir));
      break;
  }
  D(fprintf(stderr, "Capture DR; IR=0x%x, DR=0x%lx (%d bits)\n",
        ir, dr, dr_length));
}

void jtag_dtm_t::update_dr()
{
  D(fprintf(stderr, "Update DR; IR=0x%x, DR=0x%lx (%d bits)\n",
        ir, dr, dr_length));
  switch (ir) {
    case IR_DBUS:
      {
        unsigned op = get_field(dr, DBUS_OP);
        uint32_t data = get_field(dr, DBUS_DATA);
        unsigned address = get_field(dr, DBUS_ADDRESS);

        dbus = dr;

        if (op == DBUS_OP_READ || op == DBUS_OP_READ_WRITE) {
          dbus = set_field(dbus, DBUS_DATA, dm->dmi_read(address));
        }
        if (op == DBUS_OP_READ_WRITE) {
          dm->dmi_write(address, data);
        }

        dbus = set_field(dbus, DBUS_OP, DBUS_OP_STATUS_SUCCESS);
      }
      break;
  }
}
