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

  void wait_for_start();
  int wait_for_packet();

private:
  sim_t* sim;
  int tohost_fd;
  int fromhost_fd;
  bool reset;
  uint8_t seqno;

  void nack(uint8_t seqno);
  void send_packet(packet* p);
};

#endif
