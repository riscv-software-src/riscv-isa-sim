#include "processor.h"
#include "triggers.h"

namespace triggers {

mcontrol_t::mcontrol_t() :
  type(2), maskmax(0), select(false), timing(false), chain(false),
  match(MATCH_EQUAL), m(false), h(false), s(false), u(false), execute(false),
  store(false), load(false)
{
}

reg_t mcontrol_t::tdata1_read(const processor_t *proc) const noexcept {
  reg_t v = 0;
  auto xlen = proc->get_xlen();
  v = set_field(v, MCONTROL_TYPE(xlen), type);
  v = set_field(v, MCONTROL_DMODE(xlen), dmode);
  v = set_field(v, MCONTROL_MASKMAX(xlen), maskmax);
  v = set_field(v, MCONTROL_SELECT, select);
  v = set_field(v, MCONTROL_TIMING, timing);
  v = set_field(v, MCONTROL_ACTION, action);
  v = set_field(v, MCONTROL_CHAIN, chain);
  v = set_field(v, MCONTROL_MATCH, match);
  v = set_field(v, MCONTROL_M, m);
  v = set_field(v, MCONTROL_H, h);
  v = set_field(v, MCONTROL_S, s);
  v = set_field(v, MCONTROL_U, u);
  v = set_field(v, MCONTROL_EXECUTE, execute);
  v = set_field(v, MCONTROL_STORE, store);
  v = set_field(v, MCONTROL_LOAD, load);
  return v;
}

bool mcontrol_t::tdata1_write(processor_t *proc, const reg_t val) noexcept {
  if (dmode && !proc->get_state()->debug_mode) {
    return false;
  }
  auto xlen = proc->get_xlen();
  dmode = get_field(val, MCONTROL_DMODE(xlen));
  select = get_field(val, MCONTROL_SELECT);
  timing = get_field(val, MCONTROL_TIMING);
  action = (triggers::action_t) get_field(val, MCONTROL_ACTION);
  chain = get_field(val, MCONTROL_CHAIN);
  match = (triggers::mcontrol_t::match_t) get_field(val, MCONTROL_MATCH);
  m = get_field(val, MCONTROL_M);
  h = get_field(val, MCONTROL_H);
  s = get_field(val, MCONTROL_S);
  u = get_field(val, MCONTROL_U);
  execute = get_field(val, MCONTROL_EXECUTE);
  store = get_field(val, MCONTROL_STORE);
  load = get_field(val, MCONTROL_LOAD);
  // Assume we're here because of csrw.
  if (execute)
    timing = 0;
  proc->trigger_updated();
  return true;
}

module_t::module_t(unsigned count) : triggers(count) {
  for (unsigned i = 0; i < count; i++) {
    triggers[i] = new mcontrol_t();
  }
}

};
