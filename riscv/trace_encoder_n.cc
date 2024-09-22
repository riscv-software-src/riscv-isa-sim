#include "trace_encoder_n.h"

trace_encoder_n::trace_encoder_n() {
    this->trace_sink= fopen("trace_n.bin", "wb");
    this->active = true;
    this->enabled = false;
    this->src = 0;
    this->state = TRACE_ENCODER_N_IDLE;
}

void trace_encoder_n::set_enable(bool enabled) {
    bool was_enabled = this->enabled;
    printf("[trace_encoder_n] setting enable to %d\n", enabled);
    this->enabled = enabled;
    if (!was_enabled && enabled) {
        this->state = TRACE_ENCODER_N_IDLE;
    }
}

void trace_encoder_n::trace_encoder_push_commit(hart_to_encoder_ingress_t* packet) {
    printf("[trace_encoder_n] pushed commit packet at i_addr: %lx\n", packet->i_addr);
    this->packet = packet;
    if (this->enabled) {
       if (this->state == TRACE_ENCODER_N_IDLE) {
            trace_encoder_generate_packet(TCODE_PROG_TRACE_SYNC);
            this->state = TRACE_ENCODER_N_DATA;
       } else if (this->state == TRACE_ENCODER_N_DATA) {
            this->icnt += this->packet->ilastsize;
            if (this->packet->i_type == I_BRANCH_TAKEN) {
                trace_encoder_generate_packet(TCODE_DBR);
                this->icnt = 0;
            } else if (this->packet->i_type == I_JUMP_INFERABLE || this->packet->i_type == I_JUMP_UNINFERABLE) {
                trace_encoder_generate_packet(TCODE_IBR);
                this->icnt = 0;
            }
            this->state = this->icnt >= MAX_ICNT ? TRACE_ENCODER_N_FULL : TRACE_ENCODER_N_DATA;
       } else if (this->state == TRACE_ENCODER_N_FULL) {
            trace_encoder_generate_packet(TCODE_FULL);
            this->state = TRACE_ENCODER_N_DATA;
            this->icnt = 0;
       }
    } 
}

void trace_encoder_n::trace_encoder_generate_packet(tcode_t tcode) {
    trace_encoder_n_packet_t packet;
    int num_bytes;
    switch (tcode) {
        case TCODE_PROG_TRACE_SYNC:
            _set_program_trace_sync_packet(&packet);
            num_bytes = packet_to_buffer(&packet);
            fwrite(this->buffer, 1, num_bytes, this->trace_sink);
            break;
        case TCODE_DBR:
            _set_direct_branch_packet(&packet);
            num_bytes = packet_to_buffer(&packet);
            fwrite(this->buffer, 1, num_bytes, this->trace_sink);
            break;
        case TCODE_IBR:
            _set_indirect_branch_packet(&packet);
            break;
        default:
            break;
    }
}

void trace_encoder_n::_set_program_trace_sync_packet(trace_encoder_n_packet_t* packet){
    packet->tcode = TCODE_PROG_TRACE_SYNC;
    packet->src = this->src;
    packet->sync = SYNC_TRACE_EN;
    packet->icnt = 0;
    packet->f_addr = this->packet->i_addr >> 1;
}

void trace_encoder_n::_set_direct_branch_packet(trace_encoder_n_packet_t* packet){
    packet->tcode = TCODE_DBR;
    packet->src = this->src;
    packet->icnt = this->icnt;
}

void trace_encoder_n::_set_indirect_branch_packet(trace_encoder_n_packet_t* packet){
    packet->tcode = TCODE_IBR;
    packet->src = this->src;
    packet->b_type = B_INDIRECT;
    packet->icnt = this->icnt;
    uint64_t e_addr = this->packet->i_addr >> 1;
    packet->u_addr = e_addr ^ this->prev_addr;
    this->prev_addr = e_addr;
}

// returns the number of bytes written to the buffer
int trace_encoder_n::packet_to_buffer(trace_encoder_n_packet_t* packet){
    switch (packet->tcode) {
        case TCODE_PROG_TRACE_SYNC:
            return _packet_to_buffer_program_trace_sync(packet);
        case TCODE_DBR:
            return _packet_to_buffer_direct_branch_packet(packet);
        case TCODE_IBR:
            return _packet_to_buffer_indirect_branch_packet(packet);
        default:
            break;
    }
}

int trace_encoder_n::_packet_to_buffer_program_trace_sync(trace_encoder_n_packet_t* packet) {
    int msb = find_msb(packet->f_addr);
    this->buffer[0] = packet->tcode << MDO_OFFSET | MSEO_IDLE;
    this->buffer[1] = packet->sync << MDO_OFFSET | MSEO_IDLE;
    this->buffer[1] |= (packet->f_addr & 0b11) << 6;
    int num_bytes = 0;
    if (msb < 2) {
        this->buffer[1] |= MSEO_LAST;
    } else {
        packet->f_addr >>= 2;
        msb -= 2;
        num_bytes = ceil_div(msb, 6);
        for (int iter = 0; iter < num_bytes; iter++) {
            this->buffer[2 + iter] = ((packet->f_addr & 0x3F) << MDO_OFFSET) | MSEO_IDLE;
            packet->f_addr >>= 6;
        }
        this->buffer[2 + num_bytes - 1] |= MSEO_LAST;
    }
    return 2 + num_bytes;
}

int trace_encoder_n::_packet_to_buffer_direct_branch_packet(trace_encoder_n_packet_t* packet) {
    this->buffer[0] = packet->tcode << MDO_OFFSET | MSEO_IDLE;
    int msb = find_msb(packet->icnt);
    int num_bytes = ceil_div(msb, 6);
    for (int iter = 0; iter < num_bytes; iter++) {
        this->buffer[1 + iter] = ((packet->icnt & 0x3F) << MDO_OFFSET) | MSEO_IDLE;
        packet->icnt >>= 6;
    }
    this->buffer[1 + num_bytes - 1] |= MSEO_LAST;
    return 1 + num_bytes;
}

int trace_encoder_n::_packet_to_buffer_indirect_branch_packet(trace_encoder_n_packet_t* packet) {
    printf("[trace_encoder_n] _packet_to_buffer_indirect_branch_packet: packet->icnt: %lx\n", packet->icnt);
    this->buffer[0] = packet->tcode << MDO_OFFSET | MSEO_IDLE;
    this->buffer[1] = packet->b_type << MDO_OFFSET | MSEO_IDLE;
    // icnt
    int icnt_msb = find_msb(packet->icnt);
    int icnt_num_bytes = ceil_div(icnt_msb, 6);
    this->buffer[1] |= (packet->u_addr & 0xF) << 4;
    if (icnt_msb < 4) {
        this->buffer[1] |= MSEO_EOF;
    } else {
        packet->icnt >>= 4;
        icnt_msb -= 4;
        icnt_num_bytes = ceil_div(icnt_msb, 6);
        for (int iter = 0; iter < icnt_num_bytes; iter++) {
            this->buffer[2 + iter] = ((packet->icnt & 0x3F) << MDO_OFFSET) | MSEO_IDLE;
            packet->icnt >>= 6;
        }
        this->buffer[2 + icnt_num_bytes - 1] |= MSEO_EOF;
    }
    // u_addr
    int u_addr_start = 2 + icnt_num_bytes;
    int u_addr_msb = find_msb(packet->u_addr);
    int u_addr_num_bytes = ceil_div(u_addr_msb, 6);
    for (int iter = 0; iter < u_addr_num_bytes; iter++) {
        this->buffer[u_addr_start + iter] = ((packet->u_addr & 0x3F) << MDO_OFFSET) | MSEO_IDLE;
        packet->u_addr >>= 6;
    }
    this->buffer[u_addr_start + u_addr_num_bytes - 1] |= MSEO_LAST;
    return u_addr_start + u_addr_num_bytes;
}

// returns the 0-index of the most significant bit
int find_msb(uint64_t x) {
    if (x == 0) return -1;
    return 63 - __builtin_clzll(x);
}

// returns the ceiling of the division of a 0-indexed a by b
int ceil_div(int a, int b) {
    return a / b + 1;
}