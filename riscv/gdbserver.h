#ifndef _RISCV_GDBSERVER_H
#define _RISCV_GDBSERVER_H

#include <stdint.h>

template <typename T>
class circular_buffer_t
{
public:
  // The buffer can store capacity-1 data elements.
  circular_buffer_t(unsigned int capacity) : data(new T[capacity]),
      start(0), end(0), capacity(capacity) {}
  circular_buffer_t() : start(0), end(0), capacity(0) {}
  ~circular_buffer_t() { delete data; }

  T *data;
  unsigned int start;   // Data start, inclusive.
  unsigned int end;     // Data end, exclusive.
  unsigned int capacity;    // Size of the buffer.
  unsigned int size() const;
  bool empty() const { return start == end; }
  // Tell the buffer that some bytes were consumed from the start of the
  // buffer.
  void consume(unsigned int bytes);

  // Return size and address of the block of RAM where more data can be copied
  // to be added to the buffer.
  unsigned int contiguous_space() const;
  T *contiguous_data() { return data + end; }
  void data_added(unsigned int bytes);

  void reset();

  T operator[](unsigned int i) {
    return data[(start + i) % capacity];
  }
};

class gdbserver_t
{
public:
  // Create a new server, listening for connections from localhost on the given
  // port.
  gdbserver_t(uint16_t port);

  // Process all pending messages from a client.
  void handle();

  void handle_packet(const std::vector<uint8_t> &packet);

private:
  int socket_fd;
  int client_fd;
  circular_buffer_t<uint8_t> recv_buf;
  uint8_t send_buf[64 * 1024];          // Circular buffer.
  unsigned int send_start, send_end;    // Data start (inclusive)/end (exclusive)pointers.

  bool ack_mode;

  // Read pending data from the client.
  void read();
  // Accept a new client if there isn't one already connected.
  void accept();
  // Process all complete requests in recv_buf.
  void process_requests();
  // Add the given message to send_buf.
  void send(const char* msg);
};

#endif
