#ifndef REMOTE_BITBANG_H
#define REMOTE_BITBANG_H

#include <stdint.h>

#include "jtag_dtm.h"

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

class remote_bitbang_t
{
public:
  // Create a new server, listening for connections from localhost on the given
  // port.
  remote_bitbang_t(uint16_t port, jtag_dtm_t *tap);

  // Do a bit of work.
  void tick();

private:
  jtag_dtm_t *tap;

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
