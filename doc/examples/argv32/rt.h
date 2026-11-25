#pragma once

typedef unsigned u32;
typedef int i32;
typedef unsigned long long u64;
typedef long long i64;

#define SC_write 64
#define SC_getmainvars 2011
#define STDIN 0
#define STDOUT 1
#define STDERR 2

void exit(i32 code);

i32 write(i32 fd, void *data, u32 len);
void getmainvars(u32 *argcp, const char ***argvp, u32 *mem, u32 size);

u32 strlen(const char *s);

i32 print(const char *s);
i32 error(const char *s);

i32 main(u32 argc, const char **argv);
