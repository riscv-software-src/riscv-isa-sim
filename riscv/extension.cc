#include "extension.h"
#include "trap.h"
#include "dummy-rocc.h"
#include "hwacha.h"

std::map<std::string, std::function<extension_t*()>>& extensions()
{
  static std::map<std::string, std::function<extension_t*()>> v;
  return v;
}

extension_t::~extension_t()
{
}

void extension_t::illegal_instruction()
{
  throw trap_illegal_instruction();
}

void extension_t::raise_interrupt()
{
  p->set_interrupt(IRQ_COP, true);
  p->take_interrupt();
}

void extension_t::clear_interrupt()
{
  p->set_interrupt(IRQ_COP, false);
}
