#ifndef JTAG_DTM_H
#define JTAG_DTM_H

#include <stdint.h>

class debug_module_t;

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
  static const unsigned idcode = 0xdeadbeef;

  public:
    jtag_dtm_t(debug_module_t *dm);
    void reset();

    void set_pins(bool tck, bool tms, bool tdi);

    bool tdo() const { return _tdo; }

    jtag_state_t state() const { return _state; }

  private:
    debug_module_t *dm;
    bool _tck, _tms, _tdi, _tdo;
    uint32_t ir;
    const unsigned ir_length = 5;
    uint64_t dr;
    unsigned dr_length;

    // abits must come before dtmcontrol so it can easily be used in the
    // constructor.
    const unsigned abits = 6;
    uint32_t dtmcontrol;
    uint64_t dmi;

    jtag_state_t _state;

    void capture_dr();
    void update_dr();
};

#endif
