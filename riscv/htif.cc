#include "htif.h"
#include "common.h"
#include "sim.h"
#include <unistd.h>
#include <stdexcept>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stddef.h>
#include <poll.h>

enum
{
  APP_CMD_READ_MEM,
  APP_CMD_WRITE_MEM,
  APP_CMD_READ_CONTROL_REG,
  APP_CMD_WRITE_CONTROL_REG,
  APP_CMD_ACK,
  APP_CMD_NACK
};

#define APP_MAX_DATA_SIZE 1024U
#define HTIF_DATA_ALIGN 8U
struct packet
{
  reg_t cmd       :  4;
  reg_t data_size : 12;
  reg_t seqno     :  8;
  reg_t addr      : 40;

  uint64_t  data[APP_MAX_DATA_SIZE/8];
};

htif_t::htif_t(int _tohost_fd, int _fromhost_fd)
  : sim(NULL), tohost_fd(_tohost_fd), fromhost_fd(_fromhost_fd),
    reset(true), seqno(1)
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
  while (reset)
    wait_for_packet();
}

void htif_t::send_packet(packet* p)
{
  size_t data_size = p->data_size*HTIF_DATA_ALIGN;
  size_t bytes = write(tohost_fd, p, offsetof(packet,data) + data_size);
  if(bytes != offsetof(packet,data) + data_size)
  {
    const char* error = (ssize_t)bytes == -1 ? strerror(errno) : "not all bytes sent";
    fprintf(stderr,"HTIF error: %s\n", error);
    exit(-1);
  }
}

void htif_t::nack(uint8_t nack_seqno)
{
  packet p = {APP_CMD_NACK,0,nack_seqno,0};
  send_packet(&p);
}

int htif_t::wait_for_packet()
{
  while(1)
  {
    packet p;
    ssize_t bytes = read(fromhost_fd,&p,sizeof(p));
    if(bytes < (ssize_t)offsetof(packet,data))
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

    packet ackpacket = {APP_CMD_ACK,0,seqno,0};
    reg_t pcr_coreid = p.addr >> 20;
    reg_t pcr_reg = p.addr & ((1<<20)-1);

    switch(p.cmd)
    {
      case APP_CMD_READ_MEM:
        assert(p.data_size <= APP_MAX_DATA_SIZE/HTIF_DATA_ALIGN);
        assert(p.addr < sim->memsz/HTIF_DATA_ALIGN);
        assert(p.addr+p.data_size <= sim->memsz/HTIF_DATA_ALIGN);
        ackpacket.data_size = p.data_size;

        assert(HTIF_DATA_ALIGN == sizeof(uint64_t));
        for(size_t i = 0; i < p.data_size; i++)
          ackpacket.data[i] = sim->mmu->load_uint64((p.addr+i)*HTIF_DATA_ALIGN);
        break;
      case APP_CMD_WRITE_MEM:
        assert(p.data_size*HTIF_DATA_ALIGN <= bytes - offsetof(packet,data));
        assert(p.addr < sim->memsz/HTIF_DATA_ALIGN);
        assert(p.addr+p.data_size <= sim->memsz/HTIF_DATA_ALIGN);

        for(size_t i = 0; i < p.data_size; i++)
          sim->mmu->store_uint64((p.addr+i)*HTIF_DATA_ALIGN, p.data[i]);
        break;
      case APP_CMD_READ_CONTROL_REG:
      {
        assert(pcr_coreid < sim->num_cores());
        assert(p.data_size == 1);
        ackpacket.data_size = 1;
        reg_t pcr = sim->procs[pcr_coreid]->get_pcr(pcr_reg);
        memcpy(ackpacket.data, &pcr, sizeof(pcr));
        break;
      }
      case APP_CMD_WRITE_CONTROL_REG:
        assert(pcr_coreid < sim->num_cores());
        assert(p.data_size == 1);
        if (pcr_reg == PCR_RESET)
        {
          reset = p.data[0] & 1;
          sim->procs[pcr_coreid]->reset(reset);
        }
        else
        {
          reg_t pcr;
          memcpy(&pcr, p.data, sizeof(pcr));
          sim->procs[pcr_coreid]->set_pcr(pcr_reg, pcr);
        }
        break;
    }

    send_packet(&ackpacket);
    seqno++;
    return p.cmd;
  }
}

