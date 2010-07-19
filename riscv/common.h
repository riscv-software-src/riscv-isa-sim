#ifndef _RISCV_COMMON_H
#define _RISCV_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
# include <stdexcept>
# define print_and_die(s) throw std::runtime_error(s)
#else
# define print_and_die(s) do { fprintf(stderr,"%s\n",s); abort(); } while(0)
#endif

#define demand(cond,str,...) \
  do { if(!(cond)) { \
      char __str[256]; \
      snprintf(__str,256,"in %s, line %d: " str, \
               __FILE__,__LINE__,##__VA_ARGS__); \
      print_and_die(__str); \
    } } while(0)

#endif
