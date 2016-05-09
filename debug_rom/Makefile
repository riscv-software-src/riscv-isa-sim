# Recursive make is bad, but in this case we're cross compiling which is a
# pretty unusual use case.

CC = $(RISCV)/bin/riscv64-unknown-elf-gcc
OBJCOPY = $(RISCV)/bin/riscv64-unknown-elf-objcopy

%.o:	%.S
	$(CC) -I.. -c $<

debug_rom.h:	debug_rom.raw
	xxd -i $^ | sed "s/^unsigned/static const unsigned/" > $@

debug_rom.raw:	debug_rom
	$(OBJCOPY) -O binary --only-section .text debug_rom debug_rom.raw

debug_rom:	debug_rom.o link.ld
	$(CC) -nostdlib -nostartfiles -Tlink.ld -o $@ $^

clean:
	rm -f debug_rom debug_rom.o debug_rom.raw debug_rom.h
