// See LICENSE for license details.

#include "extension.h"
#include "trap.h"

extension_t::~extension_t()
{
}

void extension_t::illegal_instruction()
{
  throw trap_illegal_instruction(0);
}

void extension_t::raise_interrupt()
{
  p->take_interrupt((reg_t)1 << IRQ_COP); // must not return
  throw std::logic_error("a COP exception was posted, but interrupts are disabled!");
}

void extension_t::clear_interrupt()
{
}
