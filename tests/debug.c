#include <stdio.h>

char c = 'x';

void print_row(int length)
{
    for (int x=0; x<length; x++) {
        printf("%c", c);
    }
    printf("\n");
}

int main()
{
    volatile int i = 42;
    const char *text = "constant\n";
    int threshold = 7;

    // Wait for the debugger to get us out of this loop.
    while (i)
        ;

    printf("%s", text);
    for (int y=0; y < 10; y++) {
        print_row(y);
    }
}
