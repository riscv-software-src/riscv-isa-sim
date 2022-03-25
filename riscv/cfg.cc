// See LICENSE for license details.

#include "cfg.h"
#include "isa_parser.h"

cfg_t::~cfg_t() {}

const cfg_t &cfg_t::freeze()
{
  assert(!frozen);

  assert(!isa_parser);
  isa_parser.reset(new isa_parser_t(isa(), priv()));

  frozen = true;
  return *this;
}

cfg_arg_t<const isa_parser_t *> cfg_t::get_isa_parser() const
{
  assert(frozen && isa_parser);
  cfg_arg_t<const isa_parser_t *> ret(isa_parser.get());
  if (isa.overridden() || priv.overridden()) ret.set_overridden();
  return ret;
}
