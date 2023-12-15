


#warning bu kod _end'i nereden bilecek? diger start dosyasincan bak.

.global _start
.type   _start, @function

_start:
  # stack pointer'i ayarla
  # main'i cagir
  la sp, _end
  call main
infinity_loop:
  j infinity_loop
    