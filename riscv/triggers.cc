#include "arith.h"
#include "debug_defines.h"
#include "processor.h"
#include "triggers.h"

namespace triggers {

reg_t mcontrol_t::tdata1_read(const processor_t * const proc) const noexcept {
  reg_t v = 0;
  auto xlen = proc->get_xlen();
  v = set_field(v, MCONTROL_TYPE(xlen), MCONTROL_TYPE_MATCH);
  v = set_field(v, MCONTROL_DMODE(xlen), dmode);
  v = set_field(v, MCONTROL_MASKMAX(xlen), 0);
  v = set_field(v, CSR_MCONTROL_HIT, hit);
  v = set_field(v, MCONTROL_SELECT, select);
  v = set_field(v, MCONTROL_TIMING, timing);
  v = set_field(v, MCONTROL_ACTION, action);
  v = set_field(v, MCONTROL_CHAIN, chain_bit);
  v = set_field(v, MCONTROL_MATCH, match);
  v = set_field(v, MCONTROL_M, m);
  v = set_field(v, MCONTROL_S, s);
  v = set_field(v, MCONTROL_U, u);
  v = set_field(v, MCONTROL_EXECUTE, execute_bit);
  v = set_field(v, MCONTROL_STORE, store_bit);
  v = set_field(v, MCONTROL_LOAD, load_bit);
  return v;
}

bool mcontrol_t::tdata1_write(processor_t * const proc, const reg_t val) noexcept {
  if (dmode && !proc->get_state()->debug_mode) {
    return false;
  }
  auto xlen = proc->get_xlen();
  dmode = get_field(val, MCONTROL_DMODE(xlen));
  hit = get_field(val, CSR_MCONTROL_HIT);
  select = get_field(val, MCONTROL_SELECT);
  timing = get_field(val, MCONTROL_TIMING);
  action = (triggers::action_t) get_field(val, MCONTROL_ACTION);
  chain_bit = get_field(val, MCONTROL_CHAIN);
  unsigned match_value = get_field(val, MCONTROL_MATCH);
  switch (match_value) {
    case MATCH_EQUAL:
    case MATCH_NAPOT:
    case MATCH_GE:
    case MATCH_LT:
    case MATCH_MASK_LOW:
    case MATCH_MASK_HIGH:
      match = (triggers::mcontrol_t::match_t) match_value;
      break;
    default:
      match = MATCH_EQUAL;
      break;
  }
  m = get_field(val, MCONTROL_M);
  s = get_field(val, MCONTROL_S);
  u = get_field(val, MCONTROL_U);
  execute_bit = get_field(val, MCONTROL_EXECUTE);
  store_bit = get_field(val, MCONTROL_STORE);
  load_bit = get_field(val, MCONTROL_LOAD);
  // Assume we're here because of csrw.
  if (execute_bit)
    timing = 0;
  return true;
}

reg_t mcontrol_t::tdata2_read(const processor_t UNUSED * const proc) const noexcept {
  return tdata2;
}

bool mcontrol_t::tdata2_write(processor_t * const proc, const reg_t val) noexcept {
  if (dmode && !proc->get_state()->debug_mode) {
    return false;
  }
  tdata2 = val;
  return true;
}

bool mcontrol_t::simple_match(unsigned xlen, reg_t value) const {
  switch (match) {
    case triggers::mcontrol_t::MATCH_EQUAL:
      return value == tdata2;
    case triggers::mcontrol_t::MATCH_NAPOT:
      {
        reg_t mask = ~((1 << (cto(tdata2)+1)) - 1);
        return (value & mask) == (tdata2 & mask);
      }
    case triggers::mcontrol_t::MATCH_GE:
      return value >= tdata2;
    case triggers::mcontrol_t::MATCH_LT:
      return value < tdata2;
    case triggers::mcontrol_t::MATCH_MASK_LOW:
      {
        reg_t mask = tdata2 >> (xlen/2);
        return (value & mask) == (tdata2 & mask);
      }
    case triggers::mcontrol_t::MATCH_MASK_HIGH:
      {
        reg_t mask = tdata2 >> (xlen/2);
        return ((value >> (xlen/2)) & mask) == (tdata2 & mask);
      }
  }
  assert(0);
}

match_result_t mcontrol_t::memory_access_match(processor_t * const proc, operation_t operation, reg_t address, std::optional<reg_t> data) {
  state_t * const state = proc->get_state();
  if ((operation == triggers::OPERATION_EXECUTE && !execute_bit) ||
      (operation == triggers::OPERATION_STORE && !store_bit) ||
      (operation == triggers::OPERATION_LOAD && !load_bit) ||
      (state->prv == PRV_M && !m) ||
      (state->prv == PRV_S && !s) ||
      (state->prv == PRV_U && !u)) {
    return MATCH_NONE;
  }

  reg_t value;
  if (select) {
    if (!data.has_value())
      return MATCH_NONE;
    value = *data;
  } else {
    value = address;
  }

  // We need this because in 32-bit mode sometimes the PC bits get sign
  // extended.
  auto xlen = proc->get_xlen();
  if (xlen == 32) {
    value &= 0xffffffff;
  }

  if (simple_match(xlen, value)) {
    /* This is OK because this function is only called if the trigger was not
     * inhibited by the previous trigger in the chain. */
    hit = true;
    if (timing)
      return MATCH_FIRE_AFTER;
    else
      return MATCH_FIRE_BEFORE;
  }
  return MATCH_NONE;
}

module_t::module_t(unsigned count) : triggers(count) {
  for (unsigned i = 0; i < count; i++) {
    triggers[i] = new mcontrol_t();
  }
}

module_t::~module_t() {
  for (auto trigger : triggers) {
    delete trigger;
  }
}

match_result_t module_t::memory_access_match(action_t * const action, operation_t operation, reg_t address, std::optional<reg_t> data)
{
  state_t * const state = proc->get_state();
  if (state->debug_mode)
    return MATCH_NONE;

  bool chain_ok = true;

  for (unsigned int i = 0; i < triggers.size(); i++) {
    if (!chain_ok) {
      chain_ok |= !triggers[i]->chain();
      continue;
    }

    /* Note: We call memory_access_match for each trigger in a chain as long as
     * the triggers are matching. This results in "temperature coding" so that
     * `hit` is set on each of the consecutive triggers that matched, even if the
     * entire chain did not match. This is allowed by the spec, because the final
     * trigger in the chain will never get `hit` set unless the entire chain
     * matches. */
    match_result_t result = triggers[i]->memory_access_match(proc, operation, address, data);
    if (result != MATCH_NONE && !triggers[i]->chain()) {
      *action = triggers[i]->action;
      return result;
    }

    chain_ok = result != MATCH_NONE || !triggers[i]->chain();
  }
  return MATCH_NONE;
}

reg_t module_t::tdata1_read(const processor_t * const proc, unsigned index) const noexcept
{
  return triggers[index]->tdata1_read(proc);
}

bool module_t::tdata1_write(processor_t * const proc, unsigned index, const reg_t val) noexcept
{
  bool result = triggers[index]->tdata1_write(proc, val);
  proc->trigger_updated(triggers);
  return result;
}

reg_t module_t::tdata2_read(const processor_t * const proc, unsigned index) const noexcept
{
  return triggers[index]->tdata2_read(proc);
}

bool module_t::tdata2_write(processor_t * const proc, unsigned index, const reg_t val) noexcept
{
  bool result = triggers[index]->tdata2_write(proc, val);
  proc->trigger_updated(triggers);
  return result;
}

};
