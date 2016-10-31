#ifndef _RISCV_GDBSERVER_H
#define _RISCV_GDBSERVER_H

#include <map>
#include <queue>

#include <stdint.h>

class sim_t;

template <typename T>
class circular_buffer_t
{
public:
  // The buffer can store capacity-1 data elements.
  circular_buffer_t(unsigned int capacity) : data(new T[capacity]),
      start(0), end(0), capacity(capacity) {}
  circular_buffer_t() : start(0), end(0), capacity(0) {}
  ~circular_buffer_t() { delete[] data; }

  T *data;
  unsigned int start;   // Data start, inclusive.
  unsigned int end;     // Data end, exclusive.
  unsigned int capacity;    // Size of the buffer.
  unsigned int size() const;
  bool empty() const { return start == end; }
  bool full() const { return ((end+1) % capacity) == start; }
  T entry(unsigned index) { return data[(start + index) % capacity]; }

  // Return size and address of the block of RAM where more data can be copied
  // to be added to the buffer.
  unsigned int contiguous_empty_size() const;
  T *contiguous_empty() { return data + end; }
  void data_added(unsigned int bytes);

  unsigned int contiguous_data_size() const;
  T *contiguous_data() { return data + start; }
  // Tell the buffer that some bytes were consumed from the start of the
  // buffer.
  void consume(unsigned int bytes);

  void reset();

  T operator[](unsigned int i) const { return data[(start + i) % capacity]; }

  void append(const T *src, unsigned int count);
};

// Class to track software breakpoints that we set.
class software_breakpoint_t
{
  public:
    reg_t vaddr;
    unsigned int size;
    unsigned char instruction[4];
};

class hardware_breakpoint_t
{
  public:
    reg_t vaddr;
    unsigned int size;
    unsigned int index;
    bool load, store, execute;
};

struct hardware_breakpoint_compare_t {
  bool operator()(const hardware_breakpoint_t& a, const hardware_breakpoint_t& b) const {
    if (a.vaddr != b.vaddr)
      return a.vaddr < b.vaddr;
    return a.size < b.size;
  }
};

class gdbserver_t;

class operation_t
{
  public:
    operation_t(gdbserver_t& gdbserver) : gs(gdbserver), current_step(0) {}
    virtual ~operation_t() {}

    bool step() {
      bool result = perform_step(current_step);
      current_step++;
      return result;
    }

    // Perform the next step of this operation (which is probably to write to
    // Debug RAM and assert the debug interrupt).
    // Return true if this operation is complete. In that case the object will
    // be deleted.
    // Return false if more steps are required the next time the debug
    // interrupt is clear.
    virtual bool perform_step(unsigned int step) = 0;

  protected:
    gdbserver_t& gs;
    unsigned int current_step;
};

/*
 * word 32      64      128
 * 0    inst/0  inst/0  inst/0
 * 1    inst    inst/0  inst/0
 * 2    inst    inst    inst/0
 * 3    inst    inst    inst/0
 * 4    data0   data0   data0
 * 5    data1   data0   data0
 * 6    data2   data1   data0
 * 7            data1   data0
 * 8            data2   data1
 * 9            data2   data1
 * 10                   data1
 * 11                   data1
 * 12                   data2
 * 13                   data2
 * 14                   data2
 * 15                   data2
 */
enum slot {
  SLOT_INST0,
  SLOT_DATA0,
  SLOT_DATA1,
  SLOT_DATA_LAST,
};

static const unsigned int slot_offset32[] = {0, 4, 5, DEBUG_RAM_SIZE/4 - 1};
static const unsigned int slot_offset64[] = {0, 4, 6, DEBUG_RAM_SIZE/4 - 2};
static const unsigned int slot_offset128[] = {0, 4, 8, DEBUG_RAM_SIZE/4 - 4};

typedef enum {
  GB_SOFTWARE = 0,
  GB_HARDWARE = 1,
  GB_WRITE = 2,
  GB_READ = 3,
  GB_ACCESS = 4,
} gdb_breakpoint_type_t;

class gdbserver_t
{
public:
  // Create a new server, listening for connections from localhost on the given
  // port.
  gdbserver_t(uint16_t port, sim_t *sim);

  // Process all pending messages from a client.
  void handle();

  void handle_packet(const std::vector<uint8_t> &packet);
  void handle_interrupt();

  void software_breakpoint_remove(reg_t vaddr, unsigned int size);
  void software_breakpoint_insert(reg_t vaddr, unsigned int size);
  void hardware_breakpoint_remove(const hardware_breakpoint_t &bp);
  void hardware_breakpoint_insert(const hardware_breakpoint_t &bp);

  void handle_breakpoint(const std::vector<uint8_t> &packet);
  void handle_continue(const std::vector<uint8_t> &packet);
  void handle_extended(const std::vector<uint8_t> &packet);
  void handle_general_registers_read(const std::vector<uint8_t> &packet);
  void continue_general_registers_read();
  void handle_halt_reason(const std::vector<uint8_t> &packet);
  void handle_kill(const std::vector<uint8_t> &packet);
  void handle_memory_binary_write(const std::vector<uint8_t> &packet);
  void handle_memory_read(const std::vector<uint8_t> &packet);
  void handle_query(const std::vector<uint8_t> &packet);
  void handle_register_read(const std::vector<uint8_t> &packet);
  void continue_register_read();
  void handle_register_write(const std::vector<uint8_t> &packet);
  void handle_step(const std::vector<uint8_t> &packet);

  bool connected() const { return client_fd > 0; }

  // TODO: Move this into its own packet sending class?
  // Add the given message to send_buf.
  void send(const char* msg);
  // Hex-encode a 64-bit value, and send it to gcc in target byte order (little
  // endian).
  void send(uint64_t value);
  // Hex-encode a 32-bit value, and send it to gcc in target byte order (little
  // endian).
  void send(uint32_t value);
  // Hex-encode an 8-bit value, and send it to gcc.
  void send(uint8_t value);
  void send_packet(const char* data);
  uint8_t running_checksum;
  // Send "$" and clear running checksum.
  void start_packet();
  // Send "#" and checksum.
  void end_packet(const char* data=NULL);

  // Write value to the index'th word in Debug RAM.
  void dr_write32(unsigned int index, uint32_t value);
  void dr_write64(unsigned int index, uint64_t value);
  void dr_write(enum slot slot, uint64_t value);
  // Write jump-to-resume instruction to the index'th word in Debug RAM.
  void dr_write_jump(unsigned int index);
  // Write an xlen-bit store instruction.
  void dr_write_store(unsigned int index, unsigned int reg, enum slot);
  void dr_write_load(unsigned int index, unsigned int reg, enum slot);
  uint32_t dr_read32(unsigned int index);
  uint64_t dr_read64(unsigned int index);
  uint64_t dr_read(enum slot slot);

  uint64_t consume_hex_number_le(std::vector<uint8_t>::const_iterator &iter,
      std::vector<uint8_t>::const_iterator end);

  // Return access size to use when writing length bytes to address, so that
  // every write will be aligned.
  unsigned int find_access_size(reg_t address, int length);

  void set_interrupt(uint32_t hartid);

  // Members that ought to be privated, but that we'd really like to access
  // from operation classes.
  reg_t dpc;
  reg_t dcsr;
  reg_t mstatus;
  bool mstatus_dirty;
  reg_t sptbr;
  bool sptbr_valid;
  reg_t tselect;
  bool tselect_valid;
  bool fence_i_required;

  std::map<reg_t, reg_t> pte_cache;

  reg_t translate(reg_t vaddr);
  // Return the PRV_x that is used when the code under debug performs a memory
  // access.
  unsigned int privilege_mode();
  // Return the VM_x that is used when the code under debug performs a memory
  // access.
  unsigned int virtual_memory();

  unsigned int xlen;

  std::set<hardware_breakpoint_t, hardware_breakpoint_compare_t>
    hardware_breakpoints;

private:
  sim_t *sim;
  int socket_fd;
  int client_fd;
  circular_buffer_t<uint8_t> recv_buf;
  circular_buffer_t<uint8_t> send_buf;

  bool expect_ack;
  bool extended_mode;
  // Used to track whether we think the target is running. If we think it is
  // but it isn't, we need to tell gdb about it.
  bool running;

  std::map<reg_t, software_breakpoint_t> software_breakpoints;

  // Read pending data from the client.
  void read();
  void write();
  // Accept a new client if there isn't one already connected.
  void accept();
  // Process all complete requests in recv_buf.
  void process_requests();

  std::queue<operation_t*> operation_queue;
  void add_operation(operation_t* operation);
};

#endif
