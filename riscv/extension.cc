// See LICENSE for license details.

#include "extension.h"
#include "trap.h"

extension_t::~extension_t()
{
}

void extension_t::illegal_instruction()
{
  throw trap_illegal_instruction();
}

void extension_t::raise_interrupt()
{
  reg_t prv = p->get_state()->prv;
  reg_t mie = get_field(p->get_state()->mstatus, MSTATUS_MIE);

  if (prv < PRV_M || (prv == PRV_M && mie))
    p->raise_interrupt(IRQ_COP);

  throw std::logic_error("a COP exception was posted, but interrupts are disabled!");
}

void extension_t::clear_interrupt()
{
}
