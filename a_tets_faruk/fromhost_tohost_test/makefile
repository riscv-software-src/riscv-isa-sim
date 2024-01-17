all: .a.out .a.s

.a.out:
	riscv64-unknown-elf-gcc main.c

.a.s:
	riscv64-unknown-elf-objdump -D a.out > a.s

clean:
	rm -f a.out a.s

.PHONY: all clean run run_spike_build2

run: .a.out
	spike pk a.out

run_spike_build2: .a.out
	/home/usr1/riscv-isa-sim/build2/spike pk a.out
