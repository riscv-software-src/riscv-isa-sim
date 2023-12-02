#include <cstdint>
#include <cstdio>
#include <stdlib.h>
#include <sys/time.h>
#include <sstream>
#include <stddef.h>
#include "devices.h"
#include "processor.h"
#include "simif.h"
#include "sim.h"
#include "dts.h"
#include "mmu.h"


#define BLKDEV_ADDR      0
#define BLKDEV_OFFSET    8
#define BLKDEV_LEN       12
#define BLKDEV_WRITE     16
#define BLKDEV_REQUEST   17
#define BLKDEV_NREQUEST  18
#define BLKDEV_COMPLETE  19
#define BLKDEV_NCOMPLETE 20
#define BLKDEV_NSECTORS  24
#define BLKDEV_MAX_REQUEST_LENGTH 28
#define BLKDEV_SECTOR_SIZE 512
#define BLKDEV_SECTOR_SHIFT 9

#define MAX_REQUEST_LENGTH 16

/* #define DEBUG_BLKDEV */

#ifdef DEBUG_BLKDEV
#define blkdev_printf(...)       \
  fprintf(stderr, __VA_ARGS__);  \
  fflush(stderr);
#else
#define blkdev_printf(...)       \
  {}
#endif

blockdev_t::blockdev_t(
      const simif_t* sim,
      abstract_interrupt_controller_t *intctrl,
      uint32_t interrupt_id,
      const char* img_path)
  : sim(sim), intctrl(intctrl), interrupt_id(interrupt_id)
{
  if (!img_path) {
    blockdevice_size = (sizeof(uint64_t)/sizeof(uint8_t)) * BLKDEV_SECTOR_SIZE * 8;
    blockdevice = (uint64_t*)malloc(sizeof(uint64_t) * blockdevice_size);
  } else {
    FILE* fp = fopen(img_path, "r");
    if (fp == nullptr) {
      printf("Error opening file %s\n", img_path);
      exit(1);
    }

    fseek(fp, 0, SEEK_END);
    uint64_t img_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint64_t sectors_in_img = (img_sz + BLKDEV_SECTOR_SIZE) / BLKDEV_SECTOR_SIZE;
    blockdevice_size = sectors_in_img * BLKDEV_SECTOR_SIZE;
    blockdevice = (uint64_t*)malloc(blockdevice_size);

    const size_t read_bytes = fread((void*)blockdevice, sizeof(uint64_t), img_sz/sizeof(uint64_t), fp);
    fclose(fp);
  }

  for (int i = 0; i < trackers; i++) {
    idle_tags.push(i);
  }
}

blockdev_t::~blockdev_t() {
  free(blockdevice);
}

void blockdev_t::handle_request() {
  assert(req_addr % 8 == 0);
  if (req_write) handle_write_request();
  else handle_read_request();
  intctrl->set_interrupt_level(interrupt_id, 1);
}

void blockdev_t::handle_read_request() {
  mmu_t* simdram = sim->debug_mmu;
  uint64_t data;
  for (reg_t sidx = 0; sidx < req_len; sidx++) {
    for (reg_t i = 0; i < BLKDEV_SECTOR_SIZE; i += 8) {
      read_blockdevice_u64(&data, sidx + req_offset, i);
      simdram->store<uint64_t>(req_addr + sidx * BLKDEV_SECTOR_SIZE + i, data);
    }
  }
}

void blockdev_t::read_blockdevice_u64(uint64_t* data, reg_t sidx, reg_t boff) {
  assert(boff % 8 == 0);
  assert(sidx * BLKDEV_SECTOR_SIZE + boff < blockdevice_size);
  reg_t byte_idx = sidx * BLKDEV_SECTOR_SIZE + boff;
  reg_t bytes_per_elem = sizeof(uint64_t) / sizeof(uint8_t);
  reg_t blkdev_idx = byte_idx / bytes_per_elem;
  *data = blockdevice[blkdev_idx];
  blkdev_printf("blkdev rd: [%" PRIu64 "]: 0x%" PRIx64 "\n", blkdev_idx, *data);
}

void blockdev_t::handle_write_request() {
  mmu_t* simdram = sim->debug_mmu;
  for (reg_t sidx = 0; sidx < req_len; sidx++) {
    for (reg_t i = 0; i < BLKDEV_SECTOR_SIZE; i+= 8) {
      uint64_t data = simdram->load<uint64_t>(req_addr + sidx * BLKDEV_SECTOR_SIZE + i);
      write_blockdevice_u64(data, sidx, i);
    }
  }
}

void blockdev_t::write_blockdevice_u64(uint64_t data, reg_t sidx, reg_t boff) {
  reg_t byte_idx = sidx * BLKDEV_SECTOR_SIZE + boff;
  assert(byte_idx < blockdevice_size);

  reg_t bytes_per_elem = sizeof(uint64_t) / sizeof(uint8_t);
  reg_t blkdev_idx = byte_idx / bytes_per_elem;
  blockdevice[blkdev_idx] = data;
  blkdev_printf("blkdev wr: [%" PRIu64 "]: 0x%" PRIx64 "\n", blkdev_idx, data);
}

void blockdev_t::post_request() {
  pending_tags.push(idle_tags.front());
  idle_tags.pop();
}

bool blockdev_t::load(reg_t addr, size_t len, uint8_t* bytes) {
  if (len > 8) return false;

  int tag;
  switch (addr) {
    case BLKDEV_REQUEST:
      post_request();
      break;
    case BLKDEV_NREQUEST:
      read_little_endian_reg((int)idle_tags.size(), 0, len, bytes);
      break;
    case BLKDEV_NCOMPLETE:
      read_little_endian_reg((int)cmpl_tags.size(), 0, len, bytes);
      break;
    case BLKDEV_COMPLETE:
      assert(!cmpl_tags.empty());

      // return the completed tag
      tag = cmpl_tags.front();
      read_little_endian_reg(tag, 0, len, bytes);

      // lower interrupt when there is no longer completed reqs
      cmpl_tags.pop();
      idle_tags.push(tag);
      if ((int)cmpl_tags.size() == 0) {
        intctrl->set_interrupt_level(interrupt_id, 0);
      }
      break;
    case BLKDEV_NSECTORS:
      read_little_endian_reg((int)(blockdevice_size / BLKDEV_SECTOR_SIZE), 0, len, bytes);
      break;
    case BLKDEV_MAX_REQUEST_LENGTH:
      read_little_endian_reg((int)(MAX_REQUEST_LENGTH), 0, len, bytes);
      break;
    default:
      return false;
  }
  return true;
}

bool blockdev_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
  if (len > 8) return false;

  switch (addr) {
    case BLKDEV_ADDR:
      write_little_endian_reg(&req_addr, 0, len, bytes);
      break;
    case BLKDEV_OFFSET:
      write_little_endian_reg(&req_offset, 0, len, bytes);
      break;
    case BLKDEV_LEN:
      write_little_endian_reg(&req_len, 0, len, bytes);
      break;
    case BLKDEV_WRITE:
      write_little_endian_reg(&req_write, 0, len, bytes);
      break;
    default:
      return false;
  }
  return true;
}

void blockdev_t::tick(reg_t rtc_ticks) {
  if (++cur_tick % blockdevice_latency == 0) {
    cur_tick = 0;
  }

  if (cur_tick > 0 || pending_tags.empty()) return;

  handle_request();
  cmpl_tags.push(pending_tags.front());
  pending_tags.pop();
}

std::string blockdev_generate_dts(const sim_t* sim) {
  std::stringstream s;
  s << std::hex
    << "    BlkDev: blkdev-controller@" << BLKDEV_BASE << " {\n"
       "      compatible = \"ucbbar,blkdev\";\n"
       "      interrupt-parent = <&PLIC>;\n"
       "      interrupts = <" << std::dec << BLKDEV_INTERRUPT_ID;
  reg_t blkdevbs = BLKDEV_BASE;
  reg_t blkdevsz = BLKDEV_SIZE;
  s << std::hex << ">;\n"
       "      reg = <0x" << (blkdevbs >> 32) << " 0x" << (blkdevbs & (uint32_t)-1) <<
                   " 0x" << (blkdevsz >> 32) << " 0x" << (blkdevsz & (uint32_t)-1) << ">;\n"
       "    };\n";
  return s.str();
}

blockdev_t* blockdev_parse_from_fdt(const void* fdt, const sim_t* sim, reg_t* base)
{
  uint32_t blkdev_int_id;
  if (fdt_parse_blkdev(fdt, base, &blkdev_int_id, "ucbbar,blkdev") == 0) {
    abstract_interrupt_controller_t* intctrl = sim->get_intctrl();
    return new blockdev_t(sim, intctrl, blkdev_int_id, sim->img_path);
  } else {
    return nullptr;
  }
}

REGISTER_DEVICE(blockdev, blockdev_parse_from_fdt, blockdev_generate_dts)
