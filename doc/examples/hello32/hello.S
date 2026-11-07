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
