#ifndef _HWACHA_H
#define _HWACHA_H

#include "extension.h"

struct ct_state_t
{
  void reset();

  uint32_t nxpr;
  uint32_t nfpr;
  uint32_t maxvl;
  uint32_t vl;

  reg_t vf_pc;
};

struct ut_state_t
{
  void reset();

  bool run;
  regfile_t<reg_t, 32, true> XPR;
  regfile_t<reg_t, 32, false> FPR;
};

class hwacha_t : public extension_t
{
public:
  std::vector<insn_desc_t> get_instructions();
  const char* name() { return "hwacha"; }
  void reset();

  ct_state_t* get_ct_state() { return &ct_state; }
  ut_state_t* get_ut_state(int idx) { return &ut_state[idx]; }
  bool vf_active();

private:
  static const int max_uts = 2048;
  ct_state_t ct_state;
  ut_state_t ut_state[max_uts];
};

REGISTER_EXTENSION(hwacha, []() { return new hwacha_t; })

#endif
