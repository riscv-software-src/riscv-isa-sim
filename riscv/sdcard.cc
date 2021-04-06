#include "devices.h"
#include <cstdlib>
#include <string>
#include "mmc.h"
#include <cassert>
// http://www.files.e-shop.co.il/pdastore/Tech-mmc-samsung/SEC%20MMC%20SPEC%20ver09.pdf

// see page 26 of the manual above
#define MEMORY_SIZE (4ull * 1024 * 1024 * 1024)  // 4GB
#define READ_BL_LEN 15
#define BLOCK_LEN (1 << READ_BL_LEN)
#define NR_BLOCK (MEMORY_SIZE / BLOCK_LEN)
#define C_SIZE_MULT 7  // only 3 bits
#define MULT (1 << (C_SIZE_MULT + 2))
#define C_SIZE (NR_BLOCK / MULT - 1)



// This is a simple hardware implementation of linux/drivers/mmc/host/bcm2835.c
// No DMA and IRQ is supported, so the driver must be modified to start PIO
// right after sending the actual read/write commands.

enum {
  SDCMD, SDARG, SDTOUT, SDCDIV,
  SDRSP0, SDRSP1, SDRSP2, SDRSP3,
  SDHSTS, __PAD0, __PAD1, __PAD2,
  SDVDD, SDEDM, SDHCFG, SDHBCT,
  SDDATA, __PAD10, __PAD11, __PAD12,
  SDHBLC
};

static FILE *fp = NULL;
static uint32_t *base = NULL;
static uint32_t blkcnt = 0;
static uint32_t addr = 0;
static bool write_cmd = 0;

static void sdcard_io_handler(uint32_t offset, int len, bool is_write) {
  int idx = offset / 4;
  // printf("NEMU DEBUG: sdcard offset %d, len %d, write %d, data = 0x%x\n", offset, len, is_write, base[idx]);
  switch (idx) {
    case SDCMD:
      switch (base[SDCMD] & 0x3f) {
        case MMC_GO_IDLE_STATE: break;
        case MMC_SEND_OP_COND: base[SDRSP0] = 0x80ff8000; break;
        case MMC_ALL_SEND_CID:
          base[SDRSP0] = 0x00000001;
          base[SDRSP1] = 0x00000000;
          base[SDRSP2] = 0x00000000;
          base[SDRSP3] = 0x15000000;
          break;
        case 52: // ???
          break;
        case MMC_SEND_CSD:
          base[SDRSP0] = 0x92404001;
          base[SDRSP1] = 0x124b97e3 | ((C_SIZE & 0x3) << 30);
          base[SDRSP2] = 0x0f508000 | (C_SIZE >> 2) | (READ_BL_LEN << 16);
          base[SDRSP3] = 0x8c26012a;
          break;
        case MMC_SEND_EXT_CSD: break;
        case MMC_SLEEP_AWAKE: break;
        case MMC_APP_CMD: break;
        case MMC_SET_RELATIVE_ADDR: break;
        case MMC_SELECT_CARD: break;
        case MMC_SET_BLOCK_COUNT: blkcnt = base[SDARG] & 0xffff; break;
        case MMC_READ_MULTIPLE_BLOCK:
          // TODO
          addr = base[SDARG];
          fseek(fp, addr, SEEK_SET);
          //Log("reading from addr = 0x%x", base[SDARG]);
          write_cmd = false;
          break;
        case MMC_WRITE_MULTIPLE_BLOCK:
          // TODO
          addr = base[SDARG];
          fseek(fp, addr, SEEK_SET);
          //Log("writing to addr = 0x%x", base[SDARG]);
          write_cmd = true;
          break;
        case MMC_SEND_STATUS: base[SDRSP0] = base[SDRSP1] = base[SDRSP2] = base[SDRSP3] = 0; break;
        case MMC_STOP_TRANSMISSION: break;
        default:
          printf("unhandled command = %d", base[SDCMD] & 0x3f);
      }
      break;
    case SDARG:
      break;
    case SDRSP0:
    case SDRSP1:
    case SDRSP2:
    case SDRSP3:
    case SDHBCT:  // only for debug
    case SDHSTS:
    case SDHCFG:
      break;
    case SDHBLC: break; //Log("@@@@@@@@@@ block count = %d", base[SDHBLC]); break;// only for debug
    case SDDATA:
       // TODO
       if (!write_cmd) {
         fread(&base[SDDATA], 4, 1, fp);
       }
       addr += 4;
       break;
    case SDEDM: assert(!is_write); break;
    default:
      printf("offset = 0x%x(idx = %d), is_write = %d, data = 0x%x", offset, idx, is_write, base[idx]);
      //panic("unhandle offset = %d", offset);
  }
  // Log("offset = 0x%x(idx = %d), is_write = %d, data = 0x%x", offset, idx, is_write, base[idx]);
}

void sdcard_t::init() {
  base = (uint32_t *)malloc(0x80);
  memset(base, 0, 0x80);
  // add_mmio_map("sdhci", SD_MMIO, (void *)base, 0x80, sdcard_io_handler);

  base[SDEDM] = (8 << 4); // number of data in fifo

  // Assert(C_SIZE < (1 << 12), "shoule be fit in 12 bits");

  fp = fopen("/home51/cgk/spike_linux/debian.img", "r");
  //assert(fp);
}

bool sdcard_t::load(reg_t addr, size_t len, uint8_t* bytes) {
  // printf("sdcard read addr 0x%x len %d\n", addr, len);
  sdcard_io_handler(addr, len, false);
  uint8_t* base_8 = (uint8_t*)base;
  // memcpy(bytes, base_8 + addr, len);
  for (int i = 0; i < len; ++i) {
    bytes[i] = base_8[addr + i];
    //printf("base8[addr+i]=%x\n", base_8[addr+i]);
  }
  return true;
}

bool sdcard_t::store(reg_t addr, size_t len, const uint8_t* bytes) {
  // printf("sdcard write addr 0x%x len %d\n", addr, len);
  memcpy((uint8_t*)(base) + addr, bytes, len);
  sdcard_io_handler(addr, len , true);
  return true;
}

