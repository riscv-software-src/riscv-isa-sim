// See LICENSE for license details.

#include "htif.h"
#include "sim.h"
#include "encoding.h"
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

bool htif_isasim_t::tick()
{
  if (done())
    return false;

  do tick_once(); while (reset);

  return true;
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
        buf[i] = sim->debug_mmu->load_uint64((hdr.addr+i)*HTIF_DATA_ALIGN);
      send(buf, hdr.data_size * sizeof(buf[0]));
      break;
    }
    case HTIF_CMD_WRITE_MEM:
    {
      const uint64_t* buf = (const uint64_t*)p.get_payload();
      for (size_t i = 0; i < hdr.data_size; i++)
        sim->debug_mmu->store_uint64((hdr.addr+i)*HTIF_DATA_ALIGN, buf[i]);

      packet_header_t ack(HTIF_CMD_ACK, seqno, 0, 0);
      send(&ack, sizeof(ack));
      break;
    }
    case HTIF_CMD_READ_CONTROL_REG:
    case HTIF_CMD_WRITE_CONTROL_REG:
    {
      assert(hdr.data_size == 1);
      reg_t coreid = hdr.addr >> 20;
      reg_t regno = hdr.addr & ((1<<20)-1);
      uint64_t old_val, new_val = 0 /* shut up gcc */;

      packet_header_t ack(HTIF_CMD_ACK, seqno, 1, 0);
      send(&ack, sizeof(ack));

      if (coreid == 0xFFFFF) // system control register space
      {
        uint64_t scr = sim->get_scr(regno);
        send(&scr, sizeof(scr));
        break;
      }

      processor_t* proc = sim->get_core(coreid);
      bool write = hdr.cmd == HTIF_CMD_WRITE_CONTROL_REG;
      if (write)
        memcpy(&new_val, p.get_payload(), sizeof(new_val));

      // TODO mapping HTIF regno to CSR[4:0] is arbitrary; consider alternative
      switch (regno)
      {
        case CSR_HARTID & 0x1f:
          old_val = coreid;
          break;
        case CSR_TOHOST & 0x1f:
          old_val = proc->get_state()->tohost;
          if (write)
            proc->get_state()->tohost = new_val;
          break;
        case CSR_FROMHOST & 0x1f:
          old_val = proc->get_state()->fromhost;
          if (write && old_val == 0)
            proc->set_fromhost(new_val);
          break;
        case CSR_RESET & 0x1f:
          old_val = !proc->running();
          if (write)
          {
            reset = reset & (new_val & 1);
            proc->reset(new_val & 1);
          }
          break;
        default:
          abort();
      }

      send(&old_val, sizeof(old_val));
      break;
    }
    default:
      abort();
  }
  seqno++;
}

bool htif_isasim_t::done()
{
  if (reset)
    return false;
  return !sim->running();
}
