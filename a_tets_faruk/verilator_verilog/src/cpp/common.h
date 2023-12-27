#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef int32_t data_t;

extern int ind;
extern data_t last_commit;

void init_out_err_files();
int print_flush_err(const char *fmt, ...);
int print_flush_err_dated(const char *fmt, ...);
int print_flush_out(const char *fmt, ...);
int print_flush_out_dated(const char *fmt, ...);

#endif // COMMON_H
