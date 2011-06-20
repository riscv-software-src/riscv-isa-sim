#include "htif.h"
#include "common.h"
#include "sim.h"
#include <unistd.h>
#include <stdexcept>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

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

htif_t::htif_t(int _tohost_fd, int _fromhost_fd)
  : sim(NULL), tohost_fd(_tohost_fd), fromhost_fd(_fromhost_fd), seqno(1)
{
}

htif_t::~htif_t()
{
  close(tohost_fd);
  close(fromhost_fd);
}

void htif_t::init(sim_t* _sim)
{
  sim = _sim;
}

void htif_t::wait_for_start()
{
  while(wait_for_packet() != APP_CMD_START);
}

void htif_t::wait_for_fromhost_write()
{
  while(wait_for_packet() != APP_CMD_WRITE_CONTROL_REG);
}

void htif_t::send_packet(packet* p)
{
  int bytes = write(tohost_fd,p,offsetof(packet,data)+p->data_size);
  if((size_t)bytes != offsetof(packet,data) + p->data_size)
  {
    const char* error = bytes == -1 ? strerror(errno) : "not all bytes sent";
    fprintf(stderr,"HTIF error: %s\n", error);
    exit(-1);
  }
}

void htif_t::nack(uint16_t nack_seqno)
{
  packet p = {APP_CMD_NACK,nack_seqno,0,0};
  send_packet(&p);
}

int htif_t::wait_for_packet()
{
  while(1)
  {
    packet p;
    int bytes = read(fromhost_fd,&p,sizeof(p));
    if(bytes < (int)offsetof(packet,data))
    {
      const char* error = bytes == -1 ? strerror(errno) : "too few bytes read";
      fprintf(stderr,"HTIF error: %s\n", error);
      exit(-1);
    }
  
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
        sim->stop();
        break;
      case APP_CMD_READ_MEM:
        assert(p.addr % APP_DATA_ALIGN == 0);
        assert(p.data_size % APP_DATA_ALIGN == 0);
        assert(p.data_size <= APP_MAX_DATA_SIZE);
        assert(p.addr <= sim->memsz && p.addr+p.data_size <= sim->memsz);
        ackpacket.data_size = p.data_size;

        static_assert(APP_DATA_ALIGN >= sizeof(uint64_t))
        for(size_t i = 0; i < p.data_size/8; i++)
          ((uint64_t*)ackpacket.data)[i] = sim->mmu->load_uint64(p.addr+i*8);
        break;
      case APP_CMD_WRITE_MEM:
        assert(p.addr % APP_DATA_ALIGN == 0);
        assert(p.data_size % APP_DATA_ALIGN == 0);
        assert(p.data_size <= bytes - offsetof(packet,data));
        assert(p.addr <= sim->memsz && p.addr+p.data_size <= sim->memsz);

        for(size_t i = 0; i < p.data_size/8; i++)
          sim->mmu->store_uint64(p.addr+i*8, ((uint64_t*)p.data)[i]);
        break;
      case APP_CMD_READ_CONTROL_REG:
        assert(p.addr == 16);
        assert(p.data_size == sizeof(reg_t));
        ackpacket.data_size = sizeof(reg_t);
        memcpy(ackpacket.data,&sim->tohost,sizeof(reg_t));
        break;
      case APP_CMD_WRITE_CONTROL_REG:
        assert(p.addr == 17);
        assert(p.data_size == sizeof(reg_t));
        sim->tohost = 0;
        memcpy(&sim->fromhost,p.data,sizeof(reg_t));
        break;
    }

    send_packet(&ackpacket);
    seqno++;
    return p.cmd;
  }
}

