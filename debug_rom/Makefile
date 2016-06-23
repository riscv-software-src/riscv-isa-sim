# Recursive make is bad, but in this case we're cross compiling which is a
# pretty unusual use case.

CC = $(RISCV)/bin/riscv64-unknown-elf-gcc
OBJCOPY = $(RISCV)/bin/riscv64-unknown-elf-objcopy

COMPILE = $(CC) -nostdlib -nostartfiles -I.. -Tlink.ld

ELFS = debug_rom debug_rom32 debug_rom64
DEPS = debug_rom.S link.ld

all: $(patsubst %,%.h,$(ELFS))

%.h:	%.raw
	xxd -i $^ | sed "s/^unsigned/static const unsigned/" > $@

%.raw:	%
	$(OBJCOPY) -O binary --only-section .text $^ $@

debug_rom:	$(DEPS)
	$(COMPILE) -DRV32 -DRV64 -o $@ $^

debug_rom32:	$(DEPS)
	$(COMPILE) -DRV32 -DDEBUG_RAM_SIZE=28 -o $@ $^

debug_rom64:	$(DEPS)
	$(COMPILE) -DRV64 -o $@ $^

clean:
	rm -f $(ELFS) debug_rom*.raw debug_rom*.h
