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
  int priv = get_field(p->get_state()->mstatus, MSTATUS_PRV);
  int ie = get_field(p->get_state()->mstatus, MSTATUS_IE);

  if (priv < PRV_M || (priv == PRV_M && ie)) {
    p->raise_interrupt(IRQ_COP);
  }

  throw std::logic_error("a COP exception was posted, but interrupts are disabled!");
}

void extension_t::clear_interrupt()
{
}
