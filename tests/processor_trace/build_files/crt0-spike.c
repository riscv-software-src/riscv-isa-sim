/*
 * Parts of this file were based on information from:
 *      https://github.com/riscv/riscv-test-env/blob/master/v/vm.c
 *      https://github.com/riscv/riscv-isa-sim/issues/364#issuecomment-607657754
 *
 * This file it used as part of Embench, to provide the
 * functionality of the low-level C run-time start up
 * scripts, i.e. prior to main() being called.
 *
 * This is specifically designed to be use with the SPIKE
 * instruction set simulator (ISS), inasmuch as it
 * uses the HTIF (host/target interface) provided by SPIKE.
 */


#include <stdio.h>
#include <stdint.h>


/*
 * the function the crt will ultimately call
 */
extern int main(int argc, char *argv[]);

extern void main_wrapper(void);


/*
 * Following used by the HTIF (host/target interface).
 * see https://github.com/riscv/riscv-isa-sim/issues/364
 */
volatile uint64_t tohost = 0;
volatile uint64_t fromhost = 0;
#define TERMINATE(code)     ( ((code) << 1) | 0x1u )
#define TERMINATE_PASS TERMINATE(0u) /* terminate with exit-code = 0 (PASS) */
#define TERMINATE_FAIL TERMINATE(1u) /* terminate with exit-code = 1 (FAIL) */


/*
 * send a single 64-bit value from the simulated CPU to
 * the host ISS via the HTIF (host/target interface).
 */
static void do_tohost(const uint64_t tohost_value)
{
    while (tohost)
    {
        fromhost = 0;
    }
    tohost = tohost_value;
}


/*
 * send a single ASCII character from the simulated CPU to
 * the host ISS via the HTIF (host/target interface).
 *
 * This will send one character to the virtual blocking
 * character device driver on the host ISS.
 */
static void cputchar(const unsigned char x)
{
    do_tohost(
        0x1ull << 56 |      /* device 1: a blocking character device */
        0x1ull << 48 |      /* command 1: writes a single character */
        x);                 /* single character is in bits[7:0]  */
}


/*
 * send a null-terminated string from the simulated CPU to
 * the host ISS via the HTIF (host/target interface).
 *
 * This will send one whole string (excluding the terminating
 * null byte) to the virtual blocking character device driver
 * on the host ISS.
 */
static void cputstring(const char * s)
{
    while (*s)
    {
        cputchar((unsigned char)*s++);
    }
}


/*
 * send a magic value from the simulated CPU to the host
 * ISS via the HTIF (host/target interface), to instruct
 * the ISS to terminate the simulation.
 *
 * The value passed in to this function will be the
 * exit code as seen by the ISS. Typically, this will
 * be the unmodified return value from calling main().
 */
static void terminate(const int code)
{
    /*
     * request that the spike ISS terminates itself
     * (if we running under spike!)
     */
    do_tohost(TERMINATE(code));

    /*
     * in case he ISS does not terminate ...
     * ... loop forever!
     */
    while (1)
    {
        ;   /* loop forever */
    }
}


/*
 * This is the main "entry-point", used with "--entry"
 *
 * All this needs to do (I hope!) is:
 *
 *  1) set up the global pointer (GP)
 *  2) set up the stack pointer (SP)
 *  3) call main() (with no arguments)
 *  4) on return, tell the ISS we have finished
 *  5) loop forever ... if the ISS does not terminate!
 *
 * We only do steps #1 and #2 here ... the others are
 * done in the function main_wrapper(), to which we pass
 * control, once the two pointers above are initialized.
 *
 * The stack pointer starts <stack_size> bytes beyond
 * the linker symbol "_end" (and grows downwards towards
 * the end of the ".bss" section).
 */
extern void __attribute__((section(".text.startup")))
embench_spike_entry(void)
{
    const unsigned int stack_size = 0x1u << 17;   /* 128 KiB */

    __asm__ volatile (
        ".option push\n"
        ".option norelax\n"     /* initialize the GP + SP */
        "       lla    gp, __global_pointer$\n"
        "       lla    sp, _end\n"
        ".option pop\n"

        "       add    sp, sp, %0\n"   /* add stack_size */
        "       andi   sp, sp, ~7\n"   /* ensure the stack is 8-byte aligned */

        "       j      main_wrapper\n"      /* call main_wrapper() */

        : : "r" (stack_size) : "memory");
}


extern void __attribute__((section(".text.startup")))
main_wrapper(void)   /* only called from embench_spike_entry() */
{
    /* call main(), with no parameters */
    const int ret = main(0, NULL);

#if defined(SPIKE_VERBOSE)
    if (ret)    /* failed ? */
    {
        cputstring("done: ************ FAILED ************!\n");
    }
    else        /* success ? */
    {
        cputstring("done: passed\n");
    }
#endif  /* SPIKE_VERBOSE */

    /* terminate, with an exit-code of 'ret' */
    terminate(ret);
}
