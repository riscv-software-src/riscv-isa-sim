# Makefile for RISC-V Vector FP Conversion Tests with ZVFBFA Extension

# Configuration
RISCV_PREFIX ?= riscv64-unknown-elf-
SPIKE_PATH ?= spike
MEMORY_LAYOUT ?= "0x8000:0x2000,0x28000:0x1000,0x30000:0x1000,0x38000:0x1000,0x6b000:0x1000,0x80000:0x1000,0x88000:0x1000,0x140000:0x10000,0x1700000:0x10000,0x20d0000:0x2000000,0x20000000:0x20000000,0x40000000:0x20000000,0x7f000000:0x1000000000"
CC = $(RISCV_PREFIX)gcc
OBJDUMP = $(RISCV_PREFIX)objdump

# Compiler flags
CFLAGS = -march=rv64imafdcv_zvfbfmin_zvfbfwma_zvfh -mabi=lp64d \
         -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles \
         -Ttext=0x80000000

# Spike flags
SPIKE_FLAGS = -m$(MEMORY_LAYOUT)

# ISA strings for testing
ISA_FULL = rv64imafdcvh_zba_zbb_zbs_zcb_zcmop_zicond_zkr_zfa_zfbfmin_zfh_zkt_zicbop_zicbom_zicboz_zicfiss_zicfilp_zimop_zawrs_zifencei_zicsr_zihintpause_zihintntl_ziccif_zicntr_zihpm_zve32f_zvl1024b_zvfbfmin_zvfbfwma_zvkt_zvfh_zvbb_zvfbfa_zvfbfmin_zvfbfwma_zvfh_zvfofp8min

# Test files
TEST_SRC = test_zvfbfa_conversion.S
TEST_EXE = test_zvfbfa_conversion
TEST_DUMP = $(TEST_EXE).objdump

# Simple test files
SIMPLE_SRC = test_simple.S
SIMPLE_EXE = test_simple
SIMPLE_DUMP = $(SIMPLE_EXE).objdump

.PHONY: all clean test test-basic test-full test-dummy test-minimal test-vfwcvt help

all: $(TEST_EXE)

# Build the main test executable
$(TEST_EXE): $(TEST_SRC)
	@echo "Building test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Generating disassembly..."
	$(OBJDUMP) -D $@ > $(TEST_DUMP)
	@echo "Build complete: $@"

# Build dummy test executable
test_dummy: test_dummy.S
	@echo "Building dummy test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Dummy test build complete: $@"

# Build and show command for vfwcvt test (ZVFBFA extension test)
test-vfwcvt: test_vfwcvt
	@echo "ZVFBFA vfwcvt test executable built: test_vfwcvt"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=$(ISA_FULL) -l --instructions=20 test_vfwcvt"

# Build vfwcvt test executable
test_vfwcvt: test_vfwcvt.S
	@echo "Building ZVFBFA vfwcvt test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "ZVFBFA vfwcvt test build complete: $@"

# Build and show command for vfadd test (ZVFBFA extension test)
test-vfadd: test_vfadd_simple
	@echo "ZVFBFA vfadd test executable built: test_vfadd_simple"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=$(ISA_FULL) -l --instructions=42 test_vfadd_simple"

# Build vfadd test executable
test_vfadd_simple: test_vfadd_simple.S
	@echo "Building ZVFBFA vfadd test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "ZVFBFA vfadd test build complete: $@"

# Build and show command for vfsub test (ZVFBFA extension test)
test-vfsub: test_vfsub_simple
	@echo "ZVFBFA vfsub test executable built: test_vfsub_simple"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=$(ISA_FULL) -l --instructions=42 test_vfsub_simple"

# Build vfsub test executable
test_vfsub_simple: test_vfsub_simple.S
	@echo "Building ZVFBFA vfsub test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "ZVFBFA vfsub test build complete: $@"

# Build and show command for vfmin test (ZVFBFA extension test)
test-vfmin: test_vfmin_simple
	@echo "ZVFBFA vfmin test executable built: test_vfmin_simple"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=$(ISA_FULL) -l --instructions=42 test_vfmin_simple"

# Build vfmin test executable
test_vfmin_simple: test_vfmin_simple.S
	@echo "Building ZVFBFA vfmin test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "ZVFBFA vfmin test build complete: $@"

# Build and show command for vfmax test (ZVFBFA extension test)
test-vfmax: test_vfmax_simple
	@echo "ZVFBFA vfmax test executable built: test_vfmax_simple"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=$(ISA_FULL) -l --instructions=42 test_vfmax_simple"

# Build vfmax test executable
test_vfmax_simple: test_vfmax_simple.S
	@echo "Building ZVFBFA vfmax test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "ZVFBFA vfmax test build complete: $@"

# Build and show command for simple test (minimal test)
test-simple: test_simple
	@echo "Simple test executable built: test_simple"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=rv64imafdcv_zvfbfmin_zvfbfwma_zvfh -l --instructions=100 test_simple"

# Build simple test executable
test_simple: test_simple.S
	@echo "Building simple test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Simple test build complete: $@"

# Build and show command for OFP8 test (FP32 to OFP8 conversion)
test-ofp8: test_ofp8_minimal
	@echo "OFP8 test executable built: test_ofp8_minimal"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=rv64imafdcv_zvfbfmin_zvfbfwma_zvfh -l --instructions=150 test_ofp8_minimal"

# Build OFP8 test executable
test_ofp8_minimal: test_ofp8_minimal.S
	@echo "Building OFP8 test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "OFP8 test build complete: $@"

# Build and show command for OFP8 saturation test
test-ofp8-sat: test_ofp8_sat
	@echo "OFP8 saturation test executable built: test_ofp8_sat"
	@echo "To run with logging, use:"
	@echo "$(SPIKE_PATH) $(SPIKE_FLAGS) --isa=rv64imafdcv_zvfbfmin_zvfbfwma_zvfh_zvfofp8min -l --instructions=50 test_ofp8_sat"

# Build OFP8 saturation test executable
test_ofp8_sat: test_ofp8_sat.S
	@echo "Building OFP8 saturation test executable..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo "OFP8 saturation test build complete: $@"

# Build vfwcvt SEW=8 test
test-vfwcvt-sew8: test_vfwcvt_sew8
	@echo "vfwcvt SEW=8 test built: test_vfwcvt_sew8"
	@echo "To run: spike $(ISA_FULL) -l --instructions=50 test_vfwcvt_sew8"

test_vfwcvt_sew8: test_vfwcvt_sew8.S
	$(CC) $(CFLAGS) -o $@ $<

# Verify required tools
check-tools:
	@echo "Checking required tools..."
	@which $(CC) > /dev/null || (echo "Error: $(CC) not found" && exit 1)
	@which $(OBJDUMP) > /dev/null || (echo "Error: $(OBJDUMP) not found" && exit 1)
	@which $(SPIKE_PATH) > /dev/null || (echo "Error: $(SPIKE_PATH) not found" && exit 1)
	@echo "All required tools found!"

# Clean generated files
clean:
	rm -f $(TEST_EXE) $(TEST_DUMP) *.log test_altfmt* test_dummy test_simple test_minimal test_vfwcvt test_vfadd_simple test_vfsub_simple test_vfmin_simple test_vfmax_simple test_ofp8_minimal test_ofp8_sat test_vfwcvt_sew8

# Show help
help:
	@echo "RISC-V Vector FP Conversion Test Makefile"
	@echo "=========================================="
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build test executable"
	@echo "  test          - Run all tests"
	@echo "  test-basic    - Run test with basic ZVFBFA support"
	@echo "  test-full     - Run test with full ZVFBFA support"
	@echo "  test-verbose  - Run tests with detailed logging"
	@echo "  debug         - Start spike in debug mode"
	@echo "  check-encoding- Check instruction encodings in disassembly"
	@echo "  check-tools   - Verify required tools are available"
	@echo "  clean         - Remove generated files"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  RISCV_PREFIX  - RISC-V toolchain prefix (default: riscv64-unknown-elf-)"
	@echo "  SPIKE_PATH    - Path to spike simulator (default: spike)"
	@echo "  MEMORY_LAYOUT - Memory layout for spike (default: complex layout)"
	@echo ""
	@echo "Example usage:"
	@echo "  make check-tools  # Verify tools are available"
	@echo "  make all          # Build the test"
	@echo "  make test         # Run all tests"
	@echo "  make debug        # Debug with spike"
	@echo "  make test-dummy   # Run minimal dummy test"
	@echo ""
	@echo "Instructions tested:"
	@echo "  - vfwcvt.f.x.v    (widen convert signed int to float)"
	@echo "  - vfwcvt.f.xu.v   (widen convert unsigned int to float)"
	@echo "  - vfncvt.x.f.w    (narrow convert float to signed int)"
	@echo "  - vfncvt.xu.f.w   (narrow convert float to unsigned int)"
	@echo "  - vfncvt.rtz.x.f.w  (narrow convert float to signed int, RTZ)"
	@echo "  - vfncvt.rtz.xu.f.w (narrow convert float to unsigned int, RTZ)"
	@echo ""
	@echo "Test modes:"
	@echo "  - altfmt=0 (FP16 mode)"
	@echo "  - altfmt=1 (BF16 mode)"


