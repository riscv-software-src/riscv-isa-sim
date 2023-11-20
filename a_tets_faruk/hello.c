
#include <stdint.h>


volatile uint64_t tohost __attribute__((section(".htif"))) = 0;
volatile uint64_t fromhost __attribute__((section(".htif"))) = 0;


void _start() {
    int a = 0;
    for(;;a++);

}
