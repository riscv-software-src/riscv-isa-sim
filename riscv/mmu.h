#include "decode.h"
#include "trap.h"

class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz) : mem(_mem), memsz(_memsz), badvaddr(0) {}

  #define load_func(type) \
    type##_t load_##type(reg_t addr) { \
      check_align_and_bounds(addr, sizeof(type##_t), false, false); \
      return *(type##_t*)(mem+addr); \
    }

  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      check_align_and_bounds(addr, sizeof(type##_t), true, false); \
      *(type##_t*)(mem+addr) = val; \
    }

  insn_t load_insn(reg_t addr, bool rvc)
  {
    check_align_and_bounds(addr, rvc ? 2 : 4, false, true);
    uint16_t lo = *(uint16_t*)(mem+addr);
    uint16_t hi = *(uint16_t*)(mem+addr+2);

    insn_t insn; 
    insn.bits = lo | ((uint32_t)hi << 16);

    return insn;
  }

  load_func(uint8)
  load_func(uint16)
  load_func(uint32)
  load_func(uint64)

  load_func(int8)
  load_func(int16)
  load_func(int32)
  load_func(int64)

  store_func(uint8)
  store_func(uint16)
  store_func(uint32)
  store_func(uint64)

  reg_t get_badvaddr() { return badvaddr; }

private:
  char* mem;
  size_t memsz;
  reg_t badvaddr;

  void check_align(reg_t addr, int size, bool fetch)
  {
    if(addr & (size-1))
    {
      badvaddr = addr;
      if(fetch)
        throw trap_instruction_address_misaligned;
      throw trap_data_address_misaligned;
    }
  }

  void check_bounds(reg_t addr, int size, bool store, bool fetch)
  {
    if(addr >= memsz || addr + size > memsz)
    {
      badvaddr = addr;
      if(fetch)
        throw trap_instruction_access_fault;
      throw store ? trap_store_access_fault : trap_load_access_fault;
    }
  }

  void check_align_and_bounds(reg_t addr, int size, bool store, bool fetch)
  {
    check_align(addr, size, fetch);
    check_bounds(addr, size, store, fetch);
  }
};
