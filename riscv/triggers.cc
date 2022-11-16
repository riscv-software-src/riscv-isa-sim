#include "arith.h"
#include "debug_defines.h"
#include "processor.h"
#include "triggers.h"

namespace triggers {

reg_t trigger_with_tdata2_t::tdata2_read(const processor_t UNUSED * const proc) const noexcept {
  return tdata2;
}

void trigger_with_tdata2_t::tdata2_write(processor_t UNUSED * const proc, const reg_t UNUSED val) noexcept {
  tdata2 = val;
}

reg_t disabled_trigger_t::tdata1_read(const processor_t * const proc) const noexcept
{
  auto xlen = proc->get_xlen();
  reg_t tdata1 = 0;
  tdata1 = set_field(tdata1, CSR_TDATA1_TYPE(xlen), CSR_TDATA1_TYPE_DISABLED);
  tdata1 = set_field(tdata1, CSR_TDATA1_DMODE(xlen), dmode);
  return tdata1;
}

void disabled_trigger_t::tdata1_write(processor_t * const proc, const reg_t val, const bool UNUSED allow_chain) noexcept
{
  // Any supported tdata.type results in disabled trigger
  auto xlen = proc->get_xlen();
  dmode = get_field(val, CSR_TDATA1_DMODE(xlen));
}

reg_t mcontrol_t::tdata1_read(const processor_t * const proc) const noexcept {
  reg_t v = 0;
  auto xlen = proc->get_xlen();
  v = set_field(v, MCONTROL_TYPE(xlen), CSR_TDATA1_TYPE_MCONTROL);
  v = set_field(v, CSR_MCONTROL_DMODE(xlen), dmode);
  v = set_field(v, MCONTROL_MASKMAX(xlen), 0);
  v = set_field(v, CSR_MCONTROL_HIT, hit);
  v = set_field(v, MCONTROL_SELECT, select);
  v = set_field(v, MCONTROL_TIMING, timing);
  v = set_field(v, MCONTROL_ACTION, action);
  v = set_field(v, MCONTROL_CHAIN, chain);
  v = set_field(v, MCONTROL_MATCH, match);
  v = set_field(v, MCONTROL_M, m);
  v = set_field(v, MCONTROL_S, s);
  v = set_field(v, MCONTROL_U, u);
  v = set_field(v, MCONTROL_EXECUTE, execute);
  v = set_field(v, MCONTROL_STORE, store);
  v = set_field(v, MCONTROL_LOAD, load);
  return v;
}

void mcontrol_t::tdata1_write(processor_t * const proc, const reg_t val, const bool allow_chain) noexcept {
  auto xlen = proc->get_xlen();
  assert(get_field(val, CSR_MCONTROL_TYPE(xlen)) == CSR_TDATA1_TYPE_MCONTROL);
  dmode = get_field(val, CSR_MCONTROL_DMODE(xlen));
  hit = get_field(val, CSR_MCONTROL_HIT);
  select = get_field(val, MCONTROL_SELECT);
  timing = get_field(val, MCONTROL_TIMING);
  action = (triggers::action_t) get_field(val, MCONTROL_ACTION);
  chain = allow_chain ? get_field(val, MCONTROL_CHAIN) : 0;
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
  execute = get_field(val, MCONTROL_EXECUTE);
  store = get_field(val, MCONTROL_STORE);
  load = get_field(val, MCONTROL_LOAD);
  // Assume we're here because of csrw.
  if (execute)
    timing = 0;
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
  if ((operation == triggers::OPERATION_EXECUTE && !execute) ||
      (operation == triggers::OPERATION_STORE && !store) ||
      (operation == triggers::OPERATION_LOAD && !load) ||
      (state->prv == PRV_M && !m) ||
      (state->prv == PRV_S && !s) ||
      (state->prv == PRV_U && !u) ||
      (state->v)) {
    return match_result_t(false);
  }

  reg_t value;
  if (select) {
    if (!data.has_value())
      return match_result_t(false);
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
    return match_result_t(true, timing_t(timing), action);
  }
  return match_result_t(false);
}

module_t::module_t(unsigned count) : triggers(count) {
  for (unsigned i = 0; i < count; i++) {
    triggers[i] = new disabled_trigger_t();
  }
}

module_t::~module_t() {
  for (auto trigger : triggers) {
    delete trigger;
  }
}

reg_t module_t::tdata1_read(const processor_t * const proc, unsigned index) const noexcept
{
  return triggers[index]->tdata1_read(proc);
}

bool module_t::tdata1_write(processor_t * const proc, unsigned index, const reg_t val) noexcept
{
  if (triggers[index]->get_dmode() && !proc->get_state()->debug_mode) {
    return false;
  }

  auto xlen = proc->get_xlen();

  // hardware should ignore writes that set dmode to 1 if the previous trigger has both dmode of 0 and chain of 1
  if (index > 0 && !triggers[index-1]->get_dmode() && triggers[index-1]->get_chain() && get_field(val, CSR_TDATA1_DMODE(xlen)))
    return false;

  unsigned type = get_field(val, CSR_TDATA1_TYPE(xlen));
  reg_t tdata1 = val;
  reg_t tdata2 = triggers[index]->tdata2_read(proc);

  // hardware must zero chain in writes that set dmode to 0 if the next trigger has dmode of 1
  const bool allow_chain = !(index+1 < triggers.size() && triggers[index+1]->get_dmode() && !get_field(val, CSR_TDATA1_DMODE(xlen)));

  // dmode only writable from debug mode
  if (!proc->get_state()->debug_mode) {
    assert(CSR_TDATA1_DMODE(xlen) == CSR_MCONTROL_DMODE(xlen));
    tdata1 = set_field(tdata1, CSR_TDATA1_DMODE(xlen), 0);
  }

  delete triggers[index];
  switch (type) {
    case CSR_TDATA1_TYPE_MCONTROL: triggers[index] = new mcontrol_t(); break;
    default: triggers[index] = new disabled_trigger_t(); break;
  }

  triggers[index]->tdata1_write(proc, tdata1, allow_chain);
  triggers[index]->tdata2_write(proc, tdata2);
  proc->trigger_updated(triggers);
  return true;
}

reg_t module_t::tdata2_read(const processor_t * const proc, unsigned index) const noexcept
{
  return triggers[index]->tdata2_read(proc);
}

bool module_t::tdata2_write(processor_t * const proc, unsigned index, const reg_t val) noexcept
{
  if (triggers[index]->get_dmode() && !proc->get_state()->debug_mode) {
    return false;
  }
  triggers[index]->tdata2_write(proc, val);
  proc->trigger_updated(triggers);
  return true;
}

match_result_t module_t::memory_access_match(operation_t operation, reg_t address, std::optional<reg_t> data)
{
  state_t * const state = proc->get_state();
  if (state->debug_mode)
    return match_result_t(false);

  bool chain_ok = true;

  for (auto trigger: triggers) {
    if (!chain_ok) {
      chain_ok |= !trigger->get_chain();
      continue;
    }

    /* Note: We call memory_access_match for each trigger in a chain as long as
     * the triggers are matching. This results in "temperature coding" so that
     * `hit` is set on each of the consecutive triggers that matched, even if the
     * entire chain did not match. This is allowed by the spec, because the final
     * trigger in the chain will never get `hit` set unless the entire chain
     * matches. */
    match_result_t result = trigger->memory_access_match(proc, operation, address, data);
    if (result.fire && !trigger->get_chain())
      return result;

    chain_ok = result.fire || !trigger->get_chain();
  }
  return match_result_t(false);
}

reg_t module_t::tinfo_read(UNUSED const processor_t * const proc, unsigned UNUSED index) const noexcept
{
  /* In spike, every trigger supports the same types. */
  return (1 << CSR_TDATA1_TYPE_MCONTROL) | (1 << CSR_TDATA1_TYPE_DISABLED);
}

};
