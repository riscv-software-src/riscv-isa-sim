# Define paths
SRCDIR = src
OUTDIR = outputs
TEMPDIR = temp
INCDIR = inc

# # Define paths
SRCDIR = src
OUTDIR = outputs
TEMPDIR = temp
INCDIR = inc

# Include both .c and .s files
SOURCES_C = $(wildcard $(SRCDIR)/*.c)
SOURCES_S = $(wildcard $(SRCDIR)/*.S)
SOURCES = $(SOURCES_C) $(SOURCES_S)

# Object files for both .c and .s files
OBJS_C = $(patsubst $(SRCDIR)/%.c,$(TEMPDIR)/%.o,$(SOURCES_C))
OBJS_S = $(patsubst $(SRCDIR)/%.S,$(TEMPDIR)/%.o,$(SOURCES_S))
OBJS = $(OBJS_C) $(OBJS_S)

EXE = $(OUTDIR)/hello.elf

# Compiler and Linker options
CC = riscv64-unknown-elf-gcc
AS = riscv64-unknown-elf-as # Assembler for .S files
LD = riscv64-unknown-elf-ld
OBJDUMP = riscv64-unknown-elf-objdump
READELF = riscv64-unknown-elf-readelf
CFLAGS = -I$(INCDIR) -c -fPIC
ASFLAGS =
LDFLAGS = -T link.ld
DUMPFLAGS = -D
READFLAGS = -a

# Targets
.PHONY: all run create-objdump-readelf clean

all: $(EXE)

# Compilation rule for .c files
$(TEMPDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

# Assembly rule for .S files
$(TEMPDIR)/%.o: $(SRCDIR)/%.S
	$(AS) $(ASFLAGS) -o $@ $<

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

run: $(EXE)
	spike -d $<

create-objdump-readelf: $(EXE)
	$(OBJDUMP) $(DUMPFLAGS) $< > $(OUTDIR)/hello-dump.s
	$(READELF) $(READFLAGS) $< > $(OUTDIR)/hello-readelf.yaml

clean:
	rm -f $(TEMPDIR)/*.o $(OUTDIR)/*



# SEARCH_DIR = ../../
# EXTENSIONS = .cc .h .c

#	($(patsubst %,*%,$(EXTENSIONS)) | xargs find $(SEARCH_DIR)) 
	 
# 
# | xargs grep --color=always -En "\b$(filter-out $@,$(MAKECMDGOALS))\("

# -exec grep --color=always -Ern "\b$(filter-out $@,$(MAKECMDGOALS))\(" {} +
