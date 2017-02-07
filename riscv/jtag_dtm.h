#ifndef JTAG_DTM_H
#define JTAG_DTM_H

#include <stdint.h>

typedef enum {
  TEST_LOGIC_RESET,
  RUN_TEST_IDLE,
  SELECT_DR_SCAN,
  CAPTURE_DR,
  SHIFT_DR,
  EXIT1_DR,
  PAUSE_DR,
  EXIT2_DR,
  UPDATE_DR,
  SELECT_IR_SCAN,
  CAPTURE_IR,
  SHIFT_IR,
  EXIT1_IR,
  PAUSE_IR,
  EXIT2_IR,
  UPDATE_IR
} jtag_state_t;

class jtag_dtm_t
{
  static const unsigned idcode_ir = 1;
  static const unsigned idcode_dr = 0xdeadbeef;
  static const unsigned dtmcontrol_ir = 0x10;

  public:
    jtag_dtm_t() :
      dtmcontrol(
          (6 << 4) |        // abits
          1                 // version
          ),
      state(TEST_LOGIC_RESET) {}

    void reset() {
      state = TEST_LOGIC_RESET;
    }

    void set_pins(bool tck, bool tms, bool tdi);

    bool tdo() const { return _tdo; }

  private:
    bool _tck, _tms, _tdi, _tdo;
    uint32_t ir;
    const unsigned ir_length = 5;
    uint64_t dr;
    unsigned dr_length;

    uint32_t dtmcontrol;

    jtag_state_t state;

    void capture_dr();
    void update_dr();

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
};

#endif
