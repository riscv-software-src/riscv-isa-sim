#ifndef _RISCV_TRACE_ENCODER_N_H
#define _RISCV_TRACE_ENCODER_N_H

#include "trace_ingress.h"
#include <stdio.h>

class processor_t;

enum tcode_t {
  TCODE_OWN = 2, // ownership
  TCODE_DBR = 3, // direct branch
  TCODE_IBR = 4, // indirect branch
  TCODE_ERR = 8, // error
  TCODE_PROG_TRACE_SYNC = 9, // program trace sync
  TCODE_DBR_SYNC = 10, // direct branch sync
  TCODE_IBR_SYNC = 11, // indirect branch sync
  TCODE_FULL = 27, // resource full
  TCODE_IBR_HIST = 28, // indirect branch history
  TCODE_IBR_HIST_SYNC = 29, // indirect branch history sync
  TCODE_RBR = 30, // repeated branches
  TCODE_PROG_TRACE_CORR = 33 // program trace correlation
};

enum b_type_t {
  B_INDIRECT = 0,  // indirect branch
  B_TRAP = 1,      // trap
  B_EXCEPTION = 2, // exception
  B_INTERRUPT = 3  // interrupt
};

enum sync_t {
  SYNC_TRACE_EN = 5,
};

struct trace_encoder_n_packet_t {
  uint8_t size;    // 8 bits
  tcode_t tcode;   // 6 bits
  uint16_t src;    // unused for now
  uint8_t sync;    // 4 bit
  uint8_t b_type;  // 2 bits
  uint16_t icnt;   // 16 bits
  uint64_t f_addr; // 64 bits
  uint64_t u_addr; // 64 bits
  uint64_t tstamp; // unused for now
};

void print_packet(trace_encoder_n_packet_t* packet);
void print_encoded_packet(uint8_t* buffer, int num_bytes);

int find_msb(uint64_t x);
int ceil_div(int a, int b);

enum trace_encoder_n_state_t {
  TRACE_ENCODER_N_IDLE,
  TRACE_ENCODER_N_DATA,
  TRACE_ENCODER_N_FULL
};

#define MAX_TRACE_BUFFER_SIZE 38

#define MDO_OFFSET 2

#define MSEO_IDLE 0b00
#define MSEO_EOF  0b01
#define MSEO_RES  0b10 // reserved
#define MSEO_LAST 0b11

#define MAX_ICNT_BITS 15
#define MAX_ICNT 1 << MAX_ICNT_BITS

class trace_encoder_n {
public:

  trace_encoder_n() {
    printf("[trace_encoder_n] trace_encoder_n() constructor evaluated\n");
    this->trace_sink= fopen("trace_n.bin", "wb");
    this->debug_reference = fopen("trace_n_ref_debug.log", "wb");
    this->active = true;
    this->enabled = false;
    this->src = 0;
    this->state = TRACE_ENCODER_N_IDLE;
    this->icnt = 0;
    this->packet_0 = hart_to_encoder_ingress_t(); // create empty packet
    this->packet_1 = hart_to_encoder_ingress_t(); // create empty packet
  }
  void trace_encoder_push_commit(hart_to_encoder_ingress_t packet);
  void trace_encoder_generate_packet(tcode_t tcode);

  void set_enable(bool enabled);
  void reset();
private:
  FILE* trace_sink;
  FILE* debug_reference;

  hart_to_encoder_ingress_t packet_0; // the newer packet
  hart_to_encoder_ingress_t packet_1; // the older packet

  bool active;
  bool enabled;
  uint16_t src;
  trace_encoder_n_state_t state;
  uint8_t buffer[MAX_TRACE_BUFFER_SIZE];
  uint16_t icnt;
  uint64_t prev_addr;

  void _set_direct_branch_packet(trace_encoder_n_packet_t* packet);
  void _set_indirect_branch_packet(trace_encoder_n_packet_t* packet);
  void _set_program_trace_sync_packet(trace_encoder_n_packet_t* packet);

  int packet_to_buffer(trace_encoder_n_packet_t* packet);
  int _packet_to_buffer_program_trace_sync(trace_encoder_n_packet_t* packet);
  int _packet_to_buffer_direct_branch_packet(trace_encoder_n_packet_t* packet);
  int _packet_to_buffer_indirect_branch_packet(trace_encoder_n_packet_t* packet);

  inline hart_to_encoder_ingress_t* _get_packet_0() { return &packet_0; }
  inline hart_to_encoder_ingress_t* _get_packet_1() { return &packet_1; }
};


#endif