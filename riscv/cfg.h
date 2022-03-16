// See LICENSE for license details.
#ifndef _RISCV_CFG_H
#define _RISCV_CFG_H

#include "decode.h"

template <typename T>
class cfg_arg_t {
public:
  cfg_arg_t(T default_val)
    : value(default_val), was_set(false) {}

  bool overridden() const { return was_set; }

  T operator()() const { return value; }

  T operator=(const T v) {
    value = v;
    was_set = true;
    return value;
  }

private:
  T value;
  bool was_set;
};

class cfg_t
{
public:
  cfg_t(std::pair<reg_t, reg_t> default_initrd_bounds,
        const char *default_bootargs, size_t default_nprocs,
        const char *default_isa, const char *default_priv)
    : initrd_bounds(default_initrd_bounds),
      bootargs(default_bootargs),
      nprocs(default_nprocs),
      isa(default_isa),
      priv(default_priv)
  {}

  cfg_arg_t<std::pair<reg_t, reg_t>> initrd_bounds;
  cfg_arg_t<const char *>            bootargs;
  cfg_arg_t<size_t>                  nprocs;
  cfg_arg_t<const char *>            isa;
  cfg_arg_t<const char *>            priv;
};

#endif
