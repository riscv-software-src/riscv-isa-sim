# Define paths
SRCDIR = src
OUTDIR = outputs
TEMPDIR = temp
INCDIR = inc

# Define object files and executable names based on source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TEMPDIR)/%.o,$(SOURCES))
EXE = $(OUTDIR)/hello.elf

# Compiler and Linker options
CC = riscv64-unknown-elf-gcc
LD = riscv64-unknown-elf-ld
OBJDUMP = riscv64-unknown-elf-objdump
READELF = riscv64-unknown-elf-readelf
CFLAGS = -I$(INCDIR) -c
LDFLAGS = -T link.ld
DUMPFLAGS = -D
READFLAGS = -a

# Targets
.PHONY: all run create-objdump-readelf clean find-function-calls-defns

all: $(EXE)

$(TEMPDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: $(EXE)
	spike -d $<

create-objdump-readelf: $(EXE)
	$(OBJDUMP) $(DUMPFLAGS) $< > $(OUTDIR)/hello-dump.s
	$(READELF) $(READFLAGS) $< > $(OUTDIR)/hello-readelf.yaml

clean:
	rm -f $(TEMPDIR)/*.o $(OUTDIR)/*

find-function-calls-defs:
	@grep --color=always -Ern '\b$(filter-out $@,$(MAKECMDGOALS))\(' ../../../**
