#ifndef REMOTE_BITBANG_H
#define REMOTE_BITBANG_H

#include <stdint.h>

#define DEBUG
#ifdef DEBUG
#  define D(x) x
#else
#  define D(x)
#endif // DEBUG

class sim_t;

template <typename T>
class circular_buffer_t
{
public:
  // The buffer can store capacity-1 data elements.
  circular_buffer_t(unsigned int capacity) : data(new T[capacity]),
      start(0), end(0), capacity(capacity) {}
  circular_buffer_t() : start(0), end(0), capacity(0) {}
  ~circular_buffer_t() { delete[] data; }

  T *data;
  unsigned int start;   // Data start, inclusive.
  unsigned int end;     // Data end, exclusive.
  unsigned int capacity;    // Size of the buffer.
  unsigned int size() const;
  bool empty() const { return start == end; }
  bool full() const { return ((end+1) % capacity) == start; }
  T entry(unsigned index) { return data[(start + index) % capacity]; }

  // Return size and address of the block of RAM where more data can be copied
  // to be added to the buffer.
  unsigned int contiguous_empty_size() const;
  T *contiguous_empty() { return data + end; }
  void data_added(unsigned int bytes);

  unsigned int contiguous_data_size() const;
  T *contiguous_data() { return data + start; }
  // Tell the buffer that some bytes were consumed from the start of the
  // buffer.
  void consume(unsigned int bytes);

  void reset();

  T operator[](unsigned int i) const { return data[(start + i) % capacity]; }

  void append(const T *src, unsigned int count);
  void append(T value);
};

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

class jtag_tap_t
{
  static const unsigned idcode_ir = 1;
  static const unsigned idcode_dr = 0xdeadbeef;
  static const unsigned dtmcontrol_ir = 0x10;

  public:
    jtag_tap_t() :
      state(TEST_LOGIC_RESET) {}

    void reset() {
      state = TEST_LOGIC_RESET;
    }

    void set_pins(bool tck, bool tms, bool tdi) {
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
            break;
          case SHIFT_DR:
            _tdo = dr & 1;
            break;
          case UPDATE_DR:
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

    bool tdo() const { return _tdo; }

  private:
    bool _tck, _tms, _tdi, _tdo;
    uint32_t ir;
    const unsigned ir_length = 5;
    uint64_t dr;
    unsigned dr_length;

    uint32_t dtmcontrol = 1;

    jtag_state_t state;
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

class remote_bitbang_t
{
public:
  // Create a new server, listening for connections from localhost on the given
  // port.
  remote_bitbang_t(uint16_t port, sim_t *sim);

  // Do a bit of work.
  void tick();

private:
  jtag_tap_t tap;

  int socket_fd;
  int client_fd;
  circular_buffer_t<uint8_t> recv_buf;
  circular_buffer_t<uint8_t> send_buf;

  // Check for a client connecting, and accept if there is one.
  void accept();
  // Read as much into recv_buf as possible.
  void read();
  // Write as much of send_buf as possible.
  void write();

  // Process the input buffer.
  void process_input();
};

#endif
