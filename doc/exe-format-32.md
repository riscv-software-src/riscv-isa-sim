# 32-bit Spike programs

Spike is invoked with the general pattern
```
> spike [host options] <target program> [target options]
```

`<target program>` is a *spike program*.
If there is a `/` in `<target program>`, then that file is loaded.
If there is no `/` in `<target program>`, then spike searches for `<target program>` in the current directory and in a special system directory, made just for spike programs.
Typically, if using a regular linux distro's spike package, then the system spike program directory is `/usr/riscv64-unknown-gnu/bin`.
In general, it is `$PREFIX/$WITH_TARGET/bin`, where `$PREFIX` and `$WITH_TARGET` are the values of `--prefix` and `--with-target`, as passed to `./configure`, respectively.

A spike program is a RISC-V ELF executable, but it is not the same kind of ELF executable that would run on a full RISC-V operating system.
It is more like a kernel, running without any operating system.
Unlike a kernel, though, a spike program can communicate with spike using a custom protocol called HTIF.
See below for more on this protocol.

Here is a simple spike program:
```
/* hello.S */
.data
.align 3
.global tohost
tohost:
    .8byte 0
.global fromhost
fromhost:
    .8byte 0

message:
    .string "Hello, world!\n"
.align 3
v:
    /* write()'s syscall id, fd, data, len, unused, ... */
    .8byte 64, 1, message, 14, 0, 0, 0, 0

.text
.global _start
_start:
    la gp, __global_pointer$

    la t0, v
    sw t0, tohost, t1
wait:
    lw t0, fromhost
    beqz t0, wait

    lw t0, v

    slli t0, t0, 1
    addi t0, t0, 1
    sw t0, tohost, t1
halt:
    j halt
```
This code is also in `examples/hello32`.
It prints `Hello world!\n` to stdout, then exits with the same return value as the HTIF `write()` syscall used to print the message.
That is, the number of bytes written on success (14), and `-errno` on failure.
To run:
```
> riscv64-linux-gnu-as -mabi=ilp32 -march=rv32i hello.S -o hello.o
> riscv64-linux-gnu-ld -m elf32lriscv_ilp32 --image-base 0x80000000 -o hello hello.o
> spike --isa=RV32I --priv=M hello
Hello, world!
*** FAILED *** (tohost = 14)
> 
```
Spike counts all non-zero exit codes as failures, so this was actually a success for us.

Execution starts at the symbol `_start`, as usual for Linux.
According to the ABI, the `gp` register must be loaded with the address `__global_pointer$`, for use in linker relaxation—a process whereby code can be simplified and optimised in certain cases.

The spike program must be located in the memory after `0x80000000`, or else spike throws a `trap_load_access_fault` exception.
`ld`'s default memory origin is `0x00010000`, so that's why `--image-base 0x80000000` is passed to `ld`.
`--image-base` can be any 32-bit address greater than or equal to `0x80000000`.


## HTIF

As was mentioned above, spike programs communicate with their host with a protocol called HTIF.
This is what the `tohost` and `fromhost` symbols are for.
In RV32I, each refers to a 32-bit value.
They require 64-bit alignment and the 32 bits after each must be reserved and stay at zero, though, due to spike's implementation.
In RV32I, HTIF supports two commands: exit and syscall.

To run an *exit* command, the target does the following:
0. Write `code<<1 | 1` to `tohost`, where `code` is a signed 32-bit integer.
1. Wait forever in an infinite loop.

The host (spike) then exits with exit code `code` while the target is waiting.
Spike considers all non-zero codes to be failures.

To run a *syscall* command, the target does the following:
0. Prepare an array, `v`, of eight 64-bit integers.
   Set the first element to the syscall ID, and all subsequent elements to the arguments for that particular syscall.
1. Ensure `fromhost` is zero.
2. Write the address of the beginning of `v` to `tohost`.
3. Wait until `fromhost` is set to a non-zero value.

Upon receiving the command, the host runs the syscall, writes the result to the first element of `v`, and sets `fromhost` to a non-zero value.

## Target options

The target program accesses `[target options]` through HTIF, using the `getmainvars` syscall.
`getmainvars` takes two arguments—a memory address, `mem`, and an unsigned 32-bit integer, `size`.
The target ensures `mem` points to the beginning of a block of memory of `size` bytes.
Into this memory, the host writes the number of arguments, a `NULL`-terminated list of pointers to the argument strings, and the null-character-terminated argument strings themselves, in that order.
Due to spike's implementation, though, the number of arguments and all pointers are 64-bit numbers—not 32-bit numbers.
A full RV32I example of `getmainvars` is in `examples/argv32`.
Here is the basic idea, though:
```
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
```
