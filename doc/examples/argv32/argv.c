#include "rt.h"

i32 main(u32 argc, const char **argv)
{
    for (u32 i = 0; i < argc; i++) {
        print(argv[i]);
        print("\n");
    }
    return 0;
}
