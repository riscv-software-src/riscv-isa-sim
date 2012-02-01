#ifndef _HTIF_H
#define _HTIF_H

#include <stdint.h>

class sim_t;
struct packet;

// this class implements the host-target interface for program loading, etc.
class htif_t
{
public:
  htif_t(int _tohost_fd, int _fromhost_fd);
  ~htif_t();
  void init(sim_t* _sim);

  // wait for host to send start command
  void wait_for_start();

  // we block on the host if the target machine reads the fromhost register,
  // which provides determinism in tohost/fromhost communication.
  void wait_for_tohost_write();
  void wait_for_fromhost_write();

  // check to see if there's a pending packet and process it if so
  void poll();

private:
  sim_t* sim;
  int tohost_fd;
  int fromhost_fd;
  uint16_t seqno;

  void nack(uint16_t seqno);
  void send_packet(packet* p);
  int wait_for_packet();
};

#endif
