# Recursive make is bad, but in this case we're cross compiling which is a
# pretty unusual use case.

CC = $(RISCV)/bin/riscv64-unknown-elf-gcc
OBJCOPY = $(RISCV)/bin/riscv64-unknown-elf-objcopy

COMPILE = $(CC) -nostdlib -nostartfiles -I.. -Tlink.ld

ELFS = debug_rom
DEPS = debug_rom.S link.ld debug_rom_defines.h

all: $(patsubst %,%.h,$(ELFS))

%.h:	%.raw
	xxd -i $^ | sed "s/^unsigned/static const unsigned/" > $@

%.raw:	%
	$(OBJCOPY) -O binary --only-section .text $^ $@

debug_rom:	$(DEPS)
	$(COMPILE) -o $@ $^

clean:
	rm -f $(ELFS) debug_rom*.raw debug_rom.h
