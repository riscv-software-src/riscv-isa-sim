#include "decode.h"
#include "trap.h"

class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz) : mem(_mem), memsz(_memsz) {}

  #define load_func(type) \
    type##_t load_##type(reg_t addr) { \
      check_align_and_bounds(addr, sizeof(type##_t), false); \
      return *(type##_t*)(mem+addr); \
    }

  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      check_align_and_bounds(addr, sizeof(type##_t), false); \
      *(type##_t*)(mem+addr) = val; \
    }

  insn_t load_insn(reg_t addr)
  {
    check_align_and_bounds(addr, sizeof(insn_t), true);
    return *(insn_t*)(mem+addr);
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
      if(fetch)
        throw trap_instruction_address_misaligned;
      badvaddr = addr;
      throw trap_data_address_misaligned;
    }
  }

  void check_bounds(reg_t addr, int size, bool fetch)
  {
    if(addr >= memsz || addr + size > memsz)
    {
      if(fetch)
        throw trap_instruction_access_fault;
      badvaddr = addr;
      throw trap_data_access_fault;
    }
  }

  void check_align_and_bounds(reg_t addr, int size, bool fetch)
  {
    check_align(addr, size, fetch);
    check_bounds(addr, size, fetch);
  }
};
