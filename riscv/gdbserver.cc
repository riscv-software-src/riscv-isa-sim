#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>

#include "disasm.h"
#include "sim.h"
#include "gdbserver.h"
#include "mmu.h"

#define C_EBREAK        0x9002
#define EBREAK          0x00100073

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
    memcpy(contiguous_empty(), src, count * sizeof(T));
    data_added(count);
  }
}

gdbserver_t::gdbserver_t(uint16_t port, sim_t *sim) :
  sim(sim),
  client_fd(0),
  recv_buf(64 * 1024), send_buf(64 * 1024)
{
  // TODO: listen on socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "failed to make socket: %s (%d)\n", strerror(errno), errno);
    abort();
  }

  fcntl(socket_fd, F_SETFL, O_NONBLOCK);
  int reuseaddr = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
        sizeof(int)) == -1) {
    fprintf(stderr, "failed setsockopt: %s (%d)\n", strerror(errno), errno);
    abort();
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    fprintf(stderr, "failed to bind socket: %s (%d)\n", strerror(errno), errno);
    abort();
  }

  if (listen(socket_fd, 1) == -1) {
    fprintf(stderr, "failed to listen on socket: %s (%d)\n", strerror(errno), errno);
    abort();
  }
}

void gdbserver_t::accept()
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

    expect_ack = false;
    extended_mode = false;

    // gdb wants the core to be halted when it attaches.
    processor_t *p = sim->get_core(0);
    p->set_halted(true, HR_ATTACHED);
  }
}

void gdbserver_t::read()
{
  // Reading from a non-blocking socket still blocks if there is no data
  // available.

  size_t count = recv_buf.contiguous_empty_size();
  assert(count > 0);
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
    processor_t *p = sim->get_core(0);
    p->set_halted(false, HR_NONE);
    recv_buf.reset();
    send_buf.reset();
  } else {
    recv_buf.data_added(bytes);
  }
}

void gdbserver_t::write()
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
      printf("wrote %ld bytes: ", bytes);
      for (unsigned int i = 0; i < bytes; i++) {
        printf("%c", send_buf[i]);
      }
      printf("\n");
      send_buf.consume(bytes);
    }
  }
}

void print_packet(const std::vector<uint8_t> &packet)
{
  for (uint8_t c : packet) {
    if (c >= ' ' and c <= '~')
      fprintf(stderr, "%c", c);
    else
      fprintf(stderr, "\\x%x", c);
  }
  fprintf(stderr, "\n");
}

uint8_t compute_checksum(const std::vector<uint8_t> &packet)
{
  uint8_t checksum = 0;
  for (auto i = packet.begin() + 1; i != packet.end() - 3; i++ ) {
    checksum += *i;
  }
  return checksum;
}

uint8_t character_hex_value(uint8_t character)
{
  if (character >= '0' && character <= '9')
    return character - '0';
  if (character >= 'a' && character <= 'f')
    return 10 + character - 'a';
  if (character >= 'A' && character <= 'F')
    return 10 + character - 'A';
  return 0xff;
}

uint8_t extract_checksum(const std::vector<uint8_t> &packet)
{
  return character_hex_value(*(packet.end() - 1)) +
    16 * character_hex_value(*(packet.end() - 2));
}

void gdbserver_t::process_requests()
{
  // See https://sourceware.org/gdb/onlinedocs/gdb/Remote-Protocol.html

  while (!recv_buf.empty()) {
    std::vector<uint8_t> packet;
    for (unsigned int i = 0; i < recv_buf.size(); i++) {
      uint8_t b = recv_buf[i];

      if (packet.empty() && expect_ack && b == '+') {
        recv_buf.consume(1);
        break;
      }

      if (packet.empty() && b == 3) {
        fprintf(stderr, "Received interrupt\n");
        recv_buf.consume(1);
        handle_interrupt();
        break;
      }

      if (b == '$') {
        // Start of new packet.
        if (!packet.empty()) {
          fprintf(stderr, "Received malformed %ld-byte packet from debug client: ",
              packet.size());
          print_packet(packet);
          recv_buf.consume(i);
          break;
        }
      }

      packet.push_back(b);

      // Packets consist of $<packet-data>#<checksum>
      // where <checksum> is 
      if (packet.size() >= 4 &&
          packet[packet.size()-3] == '#') {
        handle_packet(packet);
        recv_buf.consume(i+1);
        break;
      }
    }
    // There's a partial packet in the buffer. Wait until we get more data to
    // process it.
    if (packet.size()) {
      break;
    }
  }
}

void gdbserver_t::handle_halt_reason(const std::vector<uint8_t> &packet)
{
  send_packet("S00");
}

void gdbserver_t::handle_general_registers_read(const std::vector<uint8_t> &packet)
{
  // Register order that gdb expects is:
  //   "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",
  //   "x8",  "x9",  "x10", "x11", "x12", "x13", "x14", "x15",
  //   "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
  //   "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31",
  //   "pc",
  //   "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",
  //   "f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15",
  //   "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
  //   "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",

  // Each byte of register data is described by two hex digits. The bytes with
  // the register are transmitted in target byte order. The size of each
  // register and their position within the ‘g’ packet are determined by the
  // gdb internal gdbarch functions DEPRECATED_REGISTER_RAW_SIZE and
  // gdbarch_register_name.

  send("$");
  running_checksum = 0;
  processor_t *p = sim->get_core(0);
  for (int r = 0; r < 32; r++) {
    send(p->state.XPR[r]);
  }
  send_running_checksum();
  expect_ack = true;
}

uint64_t consume_hex_number(std::vector<uint8_t>::const_iterator &iter,
    std::vector<uint8_t>::const_iterator end)
{
  uint64_t value = 0;

  while (iter != end) {
    uint8_t c = *iter;
    uint64_t c_value = character_hex_value(c);
    if (c_value > 15)
      break;
    iter++;
    value <<= 4;
    value += c_value;
  }
  return value;
}

void consume_string(std::string &str, std::vector<uint8_t>::const_iterator &iter,
    std::vector<uint8_t>::const_iterator end, uint8_t separator)
{
  while (iter != end && *iter != separator) {
    str.append(1, (char) *iter);
    iter++;
  }
}


void gdbserver_t::handle_register_read(const std::vector<uint8_t> &packet)
{
  // p n

  // Register order that gdb expects is:
  //   "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",
  //   "x8",  "x9",  "x10", "x11", "x12", "x13", "x14", "x15",
  //   "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
  //   "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31",
  //   "pc",
  //   "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",
  //   "f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15",
  //   "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
  //   "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",

  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  unsigned int n = consume_hex_number(iter, packet.end());
  if (*iter != '#')
    return send_packet("E01");

  processor_t *p = sim->get_core(0);
  send("$");
  running_checksum = 0;
  if (n < 32) {
    send(p->state.XPR[n]);
  } else if (n == 0x20) {
    send(p->state.pc);
  } else {
    send("E02");
  }

  send_running_checksum();
  expect_ack = true;
}

void gdbserver_t::handle_memory_read(const std::vector<uint8_t> &packet)
{
  // m addr,length
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  reg_t address = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E10");
  iter++;
  reg_t length = consume_hex_number(iter, packet.end());
  if (*iter != '#')
    return send_packet("E11");

  send("$");
  running_checksum = 0;
  char buffer[3];
  processor_t *p = sim->get_core(0);
  mmu_t* mmu = sim->debug_mmu;

  for (reg_t i = 0; i < length; i++) {
    sprintf(buffer, "%02x", mmu->load_uint8(address + i));
    send(buffer);
  }
  send_running_checksum();
}

void gdbserver_t::handle_memory_binary_write(const std::vector<uint8_t> &packet)
{
  // X addr,length:XX...
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  reg_t address = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E20");
  iter++;
  reg_t length = consume_hex_number(iter, packet.end());
  if (*iter != ':')
    return send_packet("E21");
  iter++;

  processor_t *p = sim->get_core(0);
  mmu_t* mmu = sim->debug_mmu;
  for (unsigned int i = 0; i < length; i++) {
    if (iter == packet.end()) {
      return send_packet("E22");
    }
    mmu->store_uint8(address + i, *iter);
    iter++;
  }
  if (*iter != '#')
    return send_packet("E4b"); // EOVERFLOW

  send_packet("OK");
}

void gdbserver_t::handle_continue(const std::vector<uint8_t> &packet)
{
  // c [addr]
  processor_t *p = sim->get_core(0);
  if (packet[2] != '#') {
    std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
    p->state.pc = consume_hex_number(iter, packet.end());
    if (*iter != '#')
      return send_packet("E30");
  }

  p->set_halted(false, HR_NONE);
  running = true;
}

void gdbserver_t::handle_step(const std::vector<uint8_t> &packet)
{
  // s [addr]
  processor_t *p = sim->get_core(0);
  if (packet[2] != '#') {
    std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
    p->state.pc = consume_hex_number(iter, packet.end());
    if (*iter != '#')
      return send_packet("E40");
  }

  p->set_single_step(true);
  running = true;
}

void gdbserver_t::handle_kill(const std::vector<uint8_t> &packet)
{
  // k
  // The exact effect of this packet is not specified.
  // Looks like OpenOCD disconnects?
  // TODO
}

void gdbserver_t::handle_extended(const std::vector<uint8_t> &packet)
{
  // Enable extended mode. In extended mode, the remote server is made
  // persistent. The ‘R’ packet is used to restart the program being debugged.
  send_packet("OK");
  extended_mode = true;
}

void software_breakpoint_t::insert(mmu_t* mmu)
{
  if (size == 2) {
    instruction = mmu->load_uint16(address);
    mmu->store_uint16(address, C_EBREAK);
  } else {
    instruction = mmu->load_uint32(address);
    mmu->store_uint32(address, EBREAK);
  }
  printf(">>> Read %x from %lx\n", instruction, address);
}

void software_breakpoint_t::remove(mmu_t* mmu)
{
  printf(">>> write %x to %lx\n", instruction, address);
  if (size == 2) {
    mmu->store_uint16(address, instruction);
  } else {
    mmu->store_uint32(address, instruction);
  }
}

void gdbserver_t::handle_breakpoint(const std::vector<uint8_t> &packet)
{
  // insert: Z type,addr,kind
  // remove: z type,addr,kind

  software_breakpoint_t bp;
  bool insert = (packet[1] == 'Z');
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;
  int type = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E50");
  iter++;
  bp.address = consume_hex_number(iter, packet.end());
  if (*iter != ',')
    return send_packet("E51");
  iter++;
  bp.size = consume_hex_number(iter, packet.end());
  // There may be more options after a ; here, but we don't support that.
  if (*iter != '#')
    return send_packet("E52");

  if (bp.size != 2 && bp.size != 4) {
    return send_packet("E53");
  }

  mmu_t* mmu = sim->debug_mmu;
  if (insert) {
    bp.insert(mmu);
    breakpoints[bp.address] = bp;

  } else {
    bp = breakpoints[bp.address];
    bp.remove(mmu);
    breakpoints.erase(bp.address);
  }
  mmu->flush_icache();
  processor_t *p = sim->get_core(0);
  p->mmu->flush_icache();
  return send_packet("OK");
}

void gdbserver_t::handle_query(const std::vector<uint8_t> &packet)
{
  std::string name;
  std::vector<uint8_t>::const_iterator iter = packet.begin() + 2;

  consume_string(name, iter, packet.end(), ':');
  if (iter != packet.end())
    iter++;
  if (name == "Supported") {
    send("$");
    running_checksum = 0;
    while (iter != packet.end()) {
      std::string feature;
      consume_string(feature, iter, packet.end(), ';');
      if (iter != packet.end())
        iter++;
      printf("is %s supported?\n", feature.c_str());
      if (feature == "swbreak+") {
        send("swbreak+;");
      }
    }
    return send_running_checksum();
  }

  printf("Unsupported query %s\n", name.c_str());
  return send_packet("");
}

void gdbserver_t::handle_packet(const std::vector<uint8_t> &packet)
{
  if (compute_checksum(packet) != extract_checksum(packet)) {
    fprintf(stderr, "Received %ld-byte packet with invalid checksum\n", packet.size());
    fprintf(stderr, "Computed checksum: %x\n", compute_checksum(packet));
    print_packet(packet);
    send("-");
    return;
  }

  fprintf(stderr, "Received %ld-byte packet from debug client: ", packet.size());
  print_packet(packet);
  send("+");

  switch (packet[1]) {
    case '!':
      return handle_extended(packet);
    case '?':
      return handle_halt_reason(packet);
    case 'g':
      return handle_general_registers_read(packet);
    case 'k':
      return handle_kill(packet);
    case 'm':
      return handle_memory_read(packet);
//    case 'M':
//      return handle_memory_write(packet);
    case 'X':
      return handle_memory_binary_write(packet);
    case 'p':
      return handle_register_read(packet);
    case 'c':
      return handle_continue(packet);
    case 's':
      return handle_step(packet);
    case 'z':
    case 'Z':
      return handle_breakpoint(packet);
    case 'q':
    case 'Q':
      return handle_query(packet);
  }

  // Not supported.
  fprintf(stderr, "** Unsupported packet: ");
  print_packet(packet);
  send_packet("");
}

void gdbserver_t::handle_interrupt()
{
  processor_t *p = sim->get_core(0);
  p->set_halted(true, HR_INTERRUPT);
  send_packet("S02");   // Pretend program received SIGINT.
  running = false;
}

void gdbserver_t::handle()
{
  processor_t *p = sim->get_core(0);
  if (running && p->halted) {
    // The core was running, but now it's halted. Better tell gdb.
    switch (p->halt_reason) {
      case HR_NONE:
        fprintf(stderr, "Internal error. Processor halted without reason.\n");
        abort();
      case HR_STEPPED:
      case HR_INTERRUPT:
      case HR_CMDLINE:
      case HR_ATTACHED:
        // There's no gdb code for this.
        send_packet("T05");
        break;
      case HR_SWBP:
        send_packet("T05swbreak:;");
        break;
    }
    send_packet("T00");
    // TODO: Actually include register values here
    running = false;
  }

  if (client_fd > 0) {
    this->read();
    this->write();

  } else {
    this->accept();
  }

  this->process_requests();
}

void gdbserver_t::send(const char* msg)
{
  unsigned int length = strlen(msg);
  for (const char *c = msg; *c; c++)
    running_checksum += *c;
  send_buf.append((const uint8_t *) msg, length);
}

void gdbserver_t::send(uint64_t value)
{
  char buffer[3];
  for (unsigned int i = 0; i < 8; i++) {
    sprintf(buffer, "%02x", (int) (value & 0xff));
    send(buffer);
    value >>= 8;
  }
}

void gdbserver_t::send(uint32_t value)
{
  char buffer[3];
  for (unsigned int i = 0; i < 4; i++) {
    sprintf(buffer, "%02x", (int) (value & 0xff));
    send(buffer);
    value >>= 8;
  }
}

void gdbserver_t::send_packet(const char* data)
{
  send("$");
  running_checksum = 0;
  send(data);
  send_running_checksum();
  expect_ack = true;
}

void gdbserver_t::send_running_checksum()
{
  char checksum_string[4];
  sprintf(checksum_string, "#%02x", running_checksum);
  send(checksum_string);
}
