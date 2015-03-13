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
  p->raise_interrupt(IRQ_COP);
}

void extension_t::clear_interrupt()
{
}
