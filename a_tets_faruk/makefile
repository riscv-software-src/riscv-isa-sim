# Define object file and executable names
OBJ = hello.o
EXE = hello.elf

# Compiler and Linker options
CC = riscv64-unknown-elf-gcc
LD = riscv64-unknown-elf-ld
OBJDUMP = riscv64-unknown-elf-objdump
READELF = riscv64-unknown-elf-readelf
CFLAGS = -c
LDFLAGS = -T link.ld
DUMPFLAGS = -D
READFLAGS = -a

# Targets
.PHONY: all run create-objdump-readelf clean

all: $(EXE)

$(OBJ): hello.c
	$(CC) $(CFLAGS) -o $(OBJ) hello.c

$(EXE): $(OBJ) link.ld
	$(LD) $(LDFLAGS) -o $(EXE) $(OBJ)

run: $(EXE)
	spike -d $(EXE)

create-objdump-readelf: $(EXE)
	$(OBJDUMP) $(DUMPFLAGS) $(EXE) > hello-dump.s;
	$(READELF) $(READFLAGS) $(EXE) > hello-readelf.yaml

clean:
	rm -f $(OBJ) $(EXE) hello-dump.s hello-readelf.yaml
