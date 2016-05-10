        .global         _start

_start:
        la      sp, stack_end
        jal     main
done:
        j       done

        .data
stack:
        .fill   4096, 1, 0
stack_end:
