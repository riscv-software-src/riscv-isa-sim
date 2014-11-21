/* include only picoc.h here - should be able to use it with only the external interfaces, no internals from interpreter.h */
#include "picoc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PICOC_STACK_SIZE (128*1024)              /* space for the the stack */

int last_spike_result;

int main(int argc, char **argv)
{
#  define HEAP_SIZE 10000000
    /* char* SourceStr = "#include <stdio.h> int main() { printf(\"Test\n\"); return 5; }"; */
    char* SourceStr = "#include<stdio.h> #include<spike.h> printf(\"Test %d %d %d\n\", reg(0,0), pc(0), mem(0)); result = 0;";
    PicocInitialise(HEAP_SIZE);
    PicocParse("nofile", SourceStr, strlen(SourceStr), TRUE, TRUE, FALSE);
    /* PicocCallMain(argc, argv); */
    PicocCleanup();

    printf("%d\n", last_spike_result);
    
    return PicocExitValue;
}
