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

#if 1
#  define D(x) x
#else
#  define D(x)
#endif

/////////// remote_bitbang_t

remote_bitbang_t::remote_bitbang_t(uint16_t port, jtag_dtm_t *tap) :
  tap(tap),
  socket_fd(0),
  client_fd(0),
  recv_start(0),
  recv_end(0)
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

  socklen_t addrlen = sizeof(addr);
  if (getsockname(socket_fd, (struct sockaddr *) &addr, &addrlen) == -1) {
    fprintf(stderr, "remote_bitbang getsockname failed: %s (%d)\n",
        strerror(errno), errno);
    abort();
  }

  printf("Listening for remote bitbang connection on port %d.\n",
      ntohs(addr.sin_port));
  fflush(stdout);
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

void remote_bitbang_t::tick()
{
  if (client_fd > 0) {
    execute_commands();
  } else {
    this->accept();
  }
}

void remote_bitbang_t::execute_commands()
{
  static char send_buf[buf_size];
  unsigned total_processed = 0;
  bool quit = false;
  bool in_rti = tap->state() == RUN_TEST_IDLE;
  bool entered_rti = false;
  while (1) {
    if (recv_start < recv_end) {
      unsigned send_offset = 0;
      while (recv_start < recv_end) {
        uint8_t command = recv_buf[recv_start];

        switch (command) {
          case 'B': /* fprintf(stderr, "*BLINK*\n"); */ break;
          case 'b': /* fprintf(stderr, "_______\n"); */ break;
          case 'r': tap->reset(); break;
          case '0': tap->set_pins(0, 0, 0); break;
          case '1': tap->set_pins(0, 0, 1); break;
          case '2': tap->set_pins(0, 1, 0); break;
          case '3': tap->set_pins(0, 1, 1); break;
          case '4': tap->set_pins(1, 0, 0); break;
          case '5': tap->set_pins(1, 0, 1); break;
          case '6': tap->set_pins(1, 1, 0); break;
          case '7': tap->set_pins(1, 1, 1); break;
          case 'R': send_buf[send_offset++] = tap->tdo() ? '1' : '0'; break;
          case 'Q': quit = true; break;
          default:
                    fprintf(stderr, "remote_bitbang got unsupported command '%c'\n",
                        command);
        }
        recv_start++;
        total_processed++;
        if (!in_rti && tap->state() == RUN_TEST_IDLE) {
          entered_rti = true;
          break;
        }
        in_rti = false;
      }
      unsigned sent = 0;
      while (sent < send_offset) {
        ssize_t bytes = write(client_fd, send_buf + sent, send_offset);
        if (bytes == -1) {
          fprintf(stderr, "failed to write to socket: %s (%d)\n", strerror(errno), errno);
          abort();
        }
        sent += bytes;
      }
    }

    if (total_processed > buf_size || quit || entered_rti) {
      // Don't go forever, because that could starve the main simulation.
      break;
    }

    recv_start = 0;
    recv_end = read(client_fd, recv_buf, buf_size);

    if (recv_end == -1) {
      if (errno == EAGAIN) {
        break;
      } else {
        fprintf(stderr, "remote_bitbang failed to read on socket: %s (%d)\n",
            strerror(errno), errno);
        abort();
      }
    }

    if (quit) {
      fprintf(stderr, "Remote Bitbang received 'Q'\n");
    }

    if (recv_end == 0 || quit) {
      // The remote disconnected.
      fprintf(stderr, "Received nothing. Quitting.\n");
      close(client_fd);
      client_fd = 0;
      break;
    }
  }
}
