
.global _start
.type   _start, @function

_start:
  # stack pointer'i ayarla
  # main'i cagir
  nop
  nop
  nop
  la sp, _stack_end
  call main
infinity_loop:
  j infinity_loop
    