#include "htif.h"
#include "common.h"
#include "sim.h"
#include <fesvr/htif-packet.h>
#include <unistd.h>
#include <stdexcept>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stddef.h>
#include <poll.h>

htif_isasim_t::htif_isasim_t(sim_t* _sim, const std::vector<std::string>& args)
  : htif_pthread_t(args), sim(_sim), reset(true), seqno(1)
{
}

void htif_isasim_t::tick()
{
  do tick_once(); while (reset);
}

void htif_isasim_t::tick_once()
{
  packet_header_t hdr;
  recv(&hdr, sizeof(hdr));

  char buf[hdr.get_packet_size()];
  memcpy(buf, &hdr, sizeof(hdr));
  recv(buf + sizeof(hdr), hdr.get_payload_size());
  packet_t p(buf);

  assert(hdr.seqno == seqno);

  switch (hdr.cmd)
  {
    case HTIF_CMD_READ_MEM:
    {
      packet_header_t ack(HTIF_CMD_ACK, seqno, hdr.data_size, 0);
      send(&ack, sizeof(ack));

      uint64_t buf[hdr.data_size];
      for (size_t i = 0; i < hdr.data_size; i++)
        buf[i] = sim->mmu->load_uint64((hdr.addr+i)*HTIF_DATA_ALIGN);
      send(buf, hdr.data_size * sizeof(buf[0]));
      break;
    }
    case HTIF_CMD_WRITE_MEM:
    {
      const uint64_t* buf = (const uint64_t*)p.get_payload();
      for (size_t i = 0; i < hdr.data_size; i++)
        sim->mmu->store_uint64((hdr.addr+i)*HTIF_DATA_ALIGN, buf[i]);

      packet_header_t ack(HTIF_CMD_ACK, seqno, 0, 0);
      send(&ack, sizeof(ack));
      break;
    }
    case HTIF_CMD_READ_CONTROL_REG:
    case HTIF_CMD_WRITE_CONTROL_REG:
    {
      reg_t coreid = hdr.addr >> 20;
      reg_t regno = hdr.addr & ((1<<20)-1);
      assert(hdr.data_size == 1);

      packet_header_t ack(HTIF_CMD_ACK, seqno, 1, 0);
      send(&ack, sizeof(ack));

      if (coreid == 0xFFFFF) // system control register space
      {
        uint64_t pcr = sim->get_scr(regno);
        send(&pcr, sizeof(pcr));
        break;
      }

      assert(coreid < sim->num_cores());
      uint64_t pcr = sim->procs[coreid]->get_pcr(regno);
      send(&pcr, sizeof(pcr));

      if (regno == PCR_TOHOST)
          sim->procs[coreid]->tohost = 0;

      if (hdr.cmd == HTIF_CMD_WRITE_CONTROL_REG)
      {
        uint64_t val;
        memcpy(&val, p.get_payload(), sizeof(val));
        if (regno == PCR_RESET)
        {
          reset = val & 1;
          sim->procs[coreid]->reset(reset);
        }
        else
        {
          sim->procs[coreid]->set_pcr(regno, val);
        }
      }
      break;
    }
    default:
      abort();
  }
  seqno++;
}

void htif_isasim_t::stop()
{
  htif_t::stop();
  exit(exit_code());
}
