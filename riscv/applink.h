#ifndef _APPLINK_H
#define _APPLINK_H

#include <stdint.h>

class sim_t;
struct packet;
class appserver_link_t
{
public:
  appserver_link_t(int _tohost_fd, int _fromhost_fd);
  void init(sim_t* _sim);
  void wait_for_start();
  void wait_for_tohost();
  void wait_for_fromhost();
  int wait_for_packet();

private:
  sim_t* sim;
  int tohost_fd;
  int fromhost_fd;
  uint16_t seqno;

  void nack(uint16_t seqno);
  void send_packet(packet* p);
};

#endif
