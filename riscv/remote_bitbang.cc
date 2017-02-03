#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstdio>

#include "remote_bitbang.h"

/////////// Circular buffer

template <typename T>
unsigned int circular_buffer_t<T>::size() const
{
  if (end >= start)
    return end - start;
  else
    return end + capacity - start;
}

template <typename T>
void circular_buffer_t<T>::consume(unsigned int bytes)
{
  start = (start + bytes) % capacity;
}

template <typename T>
unsigned int circular_buffer_t<T>::contiguous_empty_size() const
{
  if (end >= start)
    if (start == 0)
      return capacity - end - 1;
    else
      return capacity - end;
  else
    return start - end - 1;
}

template <typename T>
unsigned int circular_buffer_t<T>::contiguous_data_size() const
{
  if (end >= start)
    return end - start;
  else
    return capacity - start;
}

template <typename T>
void circular_buffer_t<T>::data_added(unsigned int bytes)
{
  end += bytes;
  assert(end <= capacity);
  if (end == capacity)
    end = 0;
}

template <typename T>
void circular_buffer_t<T>::reset()
{
  start = 0;
  end = 0;
}

template <typename T>
void circular_buffer_t<T>::append(const T *src, unsigned int count)
{
  unsigned int copy = std::min(count, contiguous_empty_size());
  memcpy(contiguous_empty(), src, copy * sizeof(T));
  data_added(copy);
  count -= copy;
  if (count > 0) {
    assert(count < contiguous_empty_size());
    memcpy(contiguous_empty(), src+copy, count * sizeof(T));
    data_added(count);
  }
}

template <typename T>
void circular_buffer_t<T>::append(T value)
{
  append(&value, 1);
}

/////////// remote_bitbang_t

remote_bitbang_t::remote_bitbang_t(uint16_t port, sim_t *sim) :
  socket_fd(0),
  client_fd(0),
  recv_buf(64 * 1024),
  send_buf(64 * 1024)
{
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "remote_bitbang failed to make socket: %s (%d)\n",
        strerror(errno), errno);
    abort();
  }

  fcntl(socket_fd, F_SETFL, O_NONBLOCK);
  int reuseaddr = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
        sizeof(int)) == -1) {
    fprintf(stderr, "remote_bitbang failed setsockopt: %s (%d)\n",
        strerror(errno), errno);
    abort();
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    fprintf(stderr, "remote_bitbang failed to bind socket: %s (%d)\n",
        strerror(errno), errno);
    abort();
  }

  if (listen(socket_fd, 1) == -1) {
    fprintf(stderr, "remote_bitbang failed to listen on socket: %s (%d)\n",
        strerror(errno), errno);
    abort();
  }
}

void remote_bitbang_t::accept()
{
  client_fd = ::accept(socket_fd, NULL, NULL);
  if (client_fd == -1) {
    if (errno == EAGAIN) {
      // No client waiting to connect right now.
    } else {
      fprintf(stderr, "failed to accept on socket: %s (%d)\n", strerror(errno),
          errno);
      abort();
    }
  } else {
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
  }
}

void remote_bitbang_t::read()
{
  // Reading from a non-blocking socket still blocks if there is no data
  // available.

  size_t count = recv_buf.contiguous_empty_size();
  ssize_t bytes = ::read(client_fd, recv_buf.contiguous_empty(), count);
  if (bytes == -1) {
    if (errno == EAGAIN) {
      // We'll try again the next call.
    } else {
      fprintf(stderr, "failed to read on socket: %s (%d)\n", strerror(errno), errno);
      abort();
    }
  } else if (bytes == 0) {
    // The remote disconnected.
    client_fd = 0;
    recv_buf.reset();
    send_buf.reset();
  } else {
    recv_buf.data_added(bytes);
    D(fprintf(stderr, "receive buffer: "));
    for (unsigned i = 0; i < recv_buf.size(); i++) {
      D(fprintf(stderr, "%c", recv_buf[i]));
    }
    D(fprintf(stderr, "\n"));
  }
}

void remote_bitbang_t::write()
{
  if (send_buf.empty())
    return;

  while (!send_buf.empty()) {
    unsigned int count = send_buf.contiguous_data_size();
    assert(count > 0);
    ssize_t bytes = ::write(client_fd, send_buf.contiguous_data(), count);
    if (bytes == -1) {
      fprintf(stderr, "failed to write to socket: %s (%d)\n", strerror(errno), errno);
      abort();
    } else if (bytes == 0) {
      // Client can't take any more data right now.
      break;
    } else {
      D(fprintf(stderr, "wrote %zd bytes: ", bytes));
      for (int i = 0; i < bytes; i++) {
        D(fprintf(stderr, "%c", send_buf[i]));
      }
      D(fprintf(stderr, "\n"));
      send_buf.consume(bytes);
    }
  }
}

void remote_bitbang_t::tick()
{
  if (client_fd > 0) {
    this->read();
    process_input();
    this->write();
  } else {
    this->accept();
  }
}

void remote_bitbang_t::process_input()
{
  for (unsigned i = 0; i < recv_buf.size(); i++) {
    uint8_t command = recv_buf[i];

    switch (command) {
      case 'B': fprintf(stderr, "*BLINK*\n"); break;
      case 'b': fprintf(stderr, "_______\n"); break;
      case 'r': tap.reset(); break;
      case '0': tap.set_pins(0, 0, 0); break;
      case '1': tap.set_pins(0, 0, 1); break;
      case '2': tap.set_pins(0, 1, 0); break;
      case '3': tap.set_pins(0, 1, 1); break;
      case '4': tap.set_pins(1, 0, 0); break;
      case '5': tap.set_pins(1, 0, 1); break;
      case '6': tap.set_pins(1, 1, 0); break;
      case '7': tap.set_pins(1, 1, 1); break;
      case 'R': send_buf.append(tap.tdo() ? '1' : '0'); break;
      default:
        fprintf(stderr, "remote_bitbang got unsupported command '%c'\n", command);
    }
  }
  recv_buf.reset();
}
