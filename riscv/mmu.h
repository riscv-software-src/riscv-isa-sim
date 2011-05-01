#include "decode.h"
#include "trap.h"
#include "icsim.h"

class processor_t;

class mmu_t
{
public:
  mmu_t(char* _mem, size_t _memsz)
   : mem(_mem), memsz(_memsz), badvaddr(0),
     icsim(NULL), dcsim(NULL), itlbsim(NULL), dtlbsim(NULL)
  {
  }

  void set_icsim(icsim_t* _icsim) { icsim = _icsim; }
  void set_dcsim(icsim_t* _dcsim) { dcsim = _dcsim; }
  void set_itlbsim(icsim_t* _itlbsim) { itlbsim = _itlbsim; }
  void set_dtlbsim(icsim_t* _dtlbsim) { dtlbsim = _dtlbsim; }

  #ifdef RISCV_ENABLE_ICSIM
  # define dcsim_tick(dcsim, dtlbsim, addr, size, st) \
      do { if(dcsim) (dcsim)->tick(addr, size, st); \
           if(dtlbsim) (dtlbsim)->tick(addr, sizeof(reg_t), false); } while(0)
  #else
  # define dcsim_tick(dcsim, addr, size)
  #endif

  #define load_func(type) \
    type##_t load_##type(reg_t addr) { \
      check_align_and_bounds(addr, sizeof(type##_t), false, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), false); \
      return *(type##_t*)(mem+addr); \
    }

  #define store_func(type) \
    void store_##type(reg_t addr, type##_t val) { \
      check_align_and_bounds(addr, sizeof(type##_t), true, false); \
      dcsim_tick(dcsim, dtlbsim, addr, sizeof(type##_t), true); \
      *(type##_t*)(mem+addr) = val; \
    }

  insn_t load_insn(reg_t addr, bool rvc)
  {
    #ifdef RISCV_ENABLE_RVC
    check_align_and_bounds(addr, rvc ? 2 : 4, false, true);
    uint16_t lo = *(uint16_t*)(mem+addr);
    uint16_t hi = *(uint16_t*)(mem+addr+2);

    insn_t insn; 
    insn.bits = lo | ((uint32_t)hi << 16);

    #ifdef RISCV_ENABLE_ICSIM
    if(icsim)
      icsim->tick(addr, insn_length(insn), false);
    if(itlbsim)
      itlbsim->tick(addr, sizeof(reg_t), false);
    #endif

    return insn;
    #else
    check_align_and_bounds(addr, 4, false, true);
    return *(insn_t*)(mem+addr);
    #endif
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

  icsim_t* icsim;
  icsim_t* dcsim;
  icsim_t* itlbsim;
  icsim_t* dtlbsim;

  void check_align(reg_t addr, int size, bool store, bool fetch)
  {
    if(addr & (size-1))
    {
      badvaddr = addr;
      if(fetch)
        throw trap_instruction_address_misaligned;
      if(store)
        throw trap_store_address_misaligned;
      throw trap_load_address_misaligned;
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
    check_align(addr, size, store, fetch);
    check_bounds(addr, size, store, fetch);
  }
  
  friend class processor_t;
};
