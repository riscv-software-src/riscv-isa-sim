test_saxpy: saxpy
	spike pk saxpy

saxpy: saxpy.c saxpy.S
	riscv64-unknown-elf-gcc -O -Wa,-march=rv64gcv saxpy.c saxpy.S -o saxpy
