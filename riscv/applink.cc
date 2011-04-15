#include "applink.h"
#include "common.h"
#include "sim.h"
#include <unistd.h>
#include <stdexcept>

enum
{
  APP_CMD_READ_MEM,
  APP_CMD_WRITE_MEM,
  APP_CMD_READ_CONTROL_REG,
  APP_CMD_WRITE_CONTROL_REG,
  APP_CMD_START,
  APP_CMD_STOP,
  APP_CMD_ACK,
  APP_CMD_NACK
};

#define APP_DATA_ALIGN 8
#define APP_MAX_DATA_SIZE 1024
struct packet
{
  uint16_t cmd;
  uint16_t seqno;
  uint32_t data_size;
  uint64_t addr;
  uint8_t  data[APP_MAX_DATA_SIZE];
};

class packet_error : public std::runtime_error
{
public:
  packet_error(const std::string& s) : std::runtime_error(s) {}
};
class io_error : public packet_error
{
public:
  io_error(const std::string& s) : packet_error(s) {}
};

appserver_link_t::appserver_link_t(int _tohost_fd, int _fromhost_fd)
  : sim(NULL), tohost_fd(_tohost_fd), fromhost_fd(_fromhost_fd), seqno(1)
{
}

void appserver_link_t::init(sim_t* _sim)
{
  sim = _sim;
}

void appserver_link_t::wait_for_start()
{
  while(wait_for_packet() != APP_CMD_START);
}

void appserver_link_t::wait_for_tohost()
{
  while(wait_for_packet() != APP_CMD_READ_CONTROL_REG);
}

void appserver_link_t::wait_for_fromhost()
{
  while(wait_for_packet() != APP_CMD_WRITE_CONTROL_REG);
}

void appserver_link_t::send_packet(packet* p)
{
  while(1) try
  {
    int bytes = write(tohost_fd,p,offsetof(packet,data)+p->data_size);
    if(bytes == -1 || (size_t)bytes != offsetof(packet,data)+p->data_size)
      throw io_error("write failed");
    return;
  }
  catch(io_error e)
  {
    fprintf(stderr,"warning: %s\n",e.what());
  }
}

void appserver_link_t::nack(uint16_t nack_seqno)
{
  packet p = {APP_CMD_NACK,nack_seqno,0,0};
  send_packet(&p);
}

int appserver_link_t::wait_for_packet()
{
  while(1) try
  {
    packet p;
    int bytes = read(fromhost_fd,&p,sizeof(p));
    if(bytes < (signed)offsetof(packet,data))
      throw io_error("read failed");
  
    if(p.seqno != seqno)
    {
      nack(p.seqno);
      continue;
    }

    packet ackpacket = {APP_CMD_ACK,seqno,0,0};

    switch(p.cmd)
    {
      case APP_CMD_START:
        break;
      case APP_CMD_STOP:
        send_packet(&ackpacket);
        throw quit_sim();
      case APP_CMD_READ_MEM:
        demand(p.addr % APP_DATA_ALIGN == 0, "misaligned address");
        demand(p.data_size % APP_DATA_ALIGN == 0, "misaligned data");
        demand(p.data_size <= APP_MAX_DATA_SIZE, "long read data");
        demand(p.addr <= sim->memsz && p.addr+p.data_size <= sim->memsz, "out of bounds");
        memcpy(ackpacket.data,sim->mem+p.addr,p.data_size);
        ackpacket.data_size = p.data_size;
        break;
      case APP_CMD_WRITE_MEM:
        demand(p.addr % APP_DATA_ALIGN == 0, "misaligned address");
        demand(p.data_size % APP_DATA_ALIGN == 0, "misaligned data");
        demand(p.data_size <= bytes - offsetof(packet,data), "short packet");
        demand(p.addr <= sim->memsz && p.addr+p.data_size <= sim->memsz, "out of bounds");
        memcpy(sim->mem+p.addr,p.data,p.data_size);
        break;
      case APP_CMD_READ_CONTROL_REG:
        demand(p.addr == 16,"bad control reg");
        demand(p.data_size == sizeof(reg_t),"bad control reg size");
        ackpacket.data_size = sizeof(reg_t);
        memcpy(ackpacket.data,&sim->tohost,sizeof(reg_t));
        break;
      case APP_CMD_WRITE_CONTROL_REG:
        demand(p.addr == 17,"bad control reg");
        demand(p.data_size == sizeof(reg_t),"bad control reg size");
        sim->tohost = 0;
        memcpy(&sim->fromhost,p.data,sizeof(reg_t));
        break;
    }

    send_packet(&ackpacket);
    seqno++;
    return p.cmd;
  }
  catch(io_error e)
  {
    fprintf(stderr,"warning: %s\n",e.what());
  }
}

