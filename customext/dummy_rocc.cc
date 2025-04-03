#include "rocc.h"
#include "mmu.h"
#include <cstring>

class dummy_rocc_t : public rocc_t
{
 public:
  const char* name() { return "dummy_rocc"; }

  reg_t custom0(rocc_insn_t insn, reg_t xs1, reg_t UNUSED xs2)
  {
    reg_t prev_acc = acc[insn.rs2];

    if (insn.rs2 >= num_acc)
      illegal_instruction();

    switch (insn.funct)
    {
      case 0: // acc <- xs1
        acc[insn.rs2] = xs1;
        break;
      case 1: // xd <- acc (the only real work is the return statement below)
        break;
      case 2: // acc[rs2] <- Mem[xs1]
        acc[insn.rs2] = p->get_mmu()->load<uint64_t>(xs1);
        break;
      case 3: // acc[rs2] <- accX + xs1
        acc[insn.rs2] += xs1;
        break;
      default:
        illegal_instruction();
    }

    return prev_acc; // in all cases, xd <- previous value of acc[rs2]
  }

  dummy_rocc_t()
  {
    memset(acc, 0, sizeof(acc));
  }

 private:
  static const int num_acc = 4;
  reg_t acc[num_acc];
};

REGISTER_EXTENSION(dummy_rocc, []() { return new dummy_rocc_t; })
