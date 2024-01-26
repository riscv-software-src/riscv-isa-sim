

// volatile long int tohost;
// volatile long int fromhost;

#define SIZE 1000

long int modular_random()
{
    static long int state = -1; // Static state variable

    // Constants for the linear congruential generator
    long int a = 1664525;    // Multiplier
    long int c = 1013904223; // Increment
    long int m = 4294967296; // Modulus (2^32)

    // Update the state using the LCG formula
    state = (a * state + c) % m;

    return state;
}
 
void main()
{
    // tohost = 2; // !!! tohost'a 1 yazince direkt programi bitiriyor.
    // !!! 2 yazinca soyle bir hata veriyor:
    // Access exception occurred while host was accessing memory
    // on behalf of target (tohost = 0x2):
    // Memory address 0x0 is invalid

//   int a[261878];
//   (*(void (*)())(&(a[261878])))(); // stack end
//   (*(void (*)())(&(a[259857])))(); // bu da calisiyor, text'te bir yere denk geliyor
    int a[SIZE];
    for (int ii = 0; ii < SIZE; ii++)
    {
        a[ii] = modular_random();
    }
    __asm(
        "nop\n"
        "nop\n");
}
