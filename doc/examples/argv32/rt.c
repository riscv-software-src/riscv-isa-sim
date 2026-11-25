#include "rt.h"


// Ensure the four bytes after tohost[0] and fromhost[0] are reserved, and start at zero, where they will stay.
u32 __attribute__((aligned(8))) tohost[2];
u32 __attribute__((aligned(8))) fromhost[2];

void exit(i32 code)
{
    tohost[0] = (code << 1) + 1;
    for (;;) ;
}

i32 syscall(u32 id, u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6)
{
    // Ensure the struct is aligned to make sure the least significant bit is zero, i.e. it is a syscall, and not an exit command.
    u64 __attribute__((aligned(8))) a[] = {
        id, arg0, arg1, arg2, arg3, arg4, arg5, arg6
    };
    fromhost[0] = 0;
    tohost[0] = (u32)(a);
    while (fromhost[0] == 0) ;
    return a[0];
}
i32 write(i32 fd, void *data, u32 len)
{
    return syscall(SC_write, (u32)(fd), (u32)(data), len, 0, 0, 0, 0);
}
void getmainvars(u32 *argcp, const char ***argvp, u32 *mem, u32 size)
{
    i32 ret = syscall(SC_getmainvars, (u32)(mem), size, 0, 0, 0, 0, 0);
    if (ret < 0) {
        error("getmainvars() failed.\n");
        exit(-ret);
    }

    // getmainvars writes an array of 64-bit numbers---not 32-bit numbers.
    // Shift everything back, by a factor of 2.
    for (u32 i = 0; mem[2*i] != 0; i++)
        mem[i] = mem[2*i];

    *argcp = mem[0];
    *argvp = (const char **)(mem + 1);
}

u32 strlen(const char *s)
{
    for (u32 i = 0; ; i++)
        if (s[i] == '\0')
            return i;
}

i32 print(const char *s)
{
    return write(STDOUT, (void *)(s), strlen(s));
}
i32 error(const char *s)
{
    return write(STDERR, (void *)(s), strlen(s));
}

void pre_main()
{
    u32 argc;
    const char **argv;
    u32 mem[1024];
    getmainvars(&argc, &argv, mem, sizeof(mem));
    exit(main(argc, argv));
}
