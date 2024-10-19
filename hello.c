#include <stdio.h>

int main()
{
    printf("Hello World\n");
    int *p = 0x41000000;
    int t = *(int *)p;
    printf("Test SR: %d\n", t);
    return 0;
}
