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

module_t::module_t(unsigned count) : triggers(count) {
  for (unsigned i = 0; i < count; i++) {
    triggers[i] = new mcontrol_t();
  }
}

};
