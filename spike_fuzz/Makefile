LIB_NAME  = spike_fuzz
BUILD_DIR = $(abspath ./build)

CC = clang
CXX = clang++

# Spike build
SPIKE_PATH = $(abspath ..)
SPIKE_CONFIGURE = $(SPIKE_PATH)/configure
SPIKE_CONFIGURE_FLAGS = -q CC=$(CC) CXX=$(CXX)                   \
	CXXFLAGS="-O3 -Wno-c99-designator"                           \
	--with-boost=no --with-boost-asio=no --with-boost-regex=no
SPIKE_MAKEFILE = $(BUILD_DIR)/Makefile

# spike_fuzz library sources
FUZZ_TARGET = $(BUILD_DIR)/lib$(LIB_NAME).a
FUZZ_SRCS   = $(shell find . -maxdepth 1 -name '*.cc')
FUZZ_OBJS   = $(addprefix $(BUILD_DIR)/, $(patsubst %.cc, %.o, $(FUZZ_SRCS)))
CXX_FLAGS   = -O3 --std=c++20 -fPIC

ifneq ($(SPIKE_DEBUG),)
CXX_FLAGS += -DLOG_PATH=\"$(BUILD_DIR)/spike.log\"
endif

SPIKE_INC   = fesvr riscv disasm customext fdt softfloat spike_main spike_dasm build
CXX_FLAGS  += -I$(SPIKE_PATH) $(addprefix -I$(SPIKE_PATH)/, $(SPIKE_INC))

# spike_fuzz elf dependencies
SPIKE_LIB = libspike_main.a libriscv.a libdisasm.a libsoftfloat.a libfesvr.a libfdt.a
ELF_LIBS  = $(addprefix $(BUILD_DIR)/, $(SPIKE_LIB))


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SPIKE_MAKEFILE): | $(BUILD_DIR)
	cd $(BUILD_DIR) && $(SPIKE_CONFIGURE) $(SPIKE_CONFIGURE_FLAGS)
	@# Remove -g from Makefile. Don't know why it's always there.
	@sed -i 's/ -g / /g' $(SPIKE_MAKEFILE)

spike: $(SPIKE_MAKEFILE)
	$(MAKE) -s -C $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cc
	$(CXX) $(CXX_FLAGS) $(INC_PATH) $< -c -o $@

$(FUZZ_TARGET): $(FUZZ_OBJS)
	ar rcs $(FUZZ_TARGET) $(FUZZ_OBJS)

libfuzz: spike $(FUZZ_TARGET)

fuzz: spike $(FUZZ_TARGET) spike_fuzz.c
	$(CC) -c spike_fuzz.c -o $(BUILD_DIR)/spike_fuzz.o
	$(CXX) $(BUILD_DIR)/spike_fuzz.o $(FUZZ_TARGET) $(ELF_LIBS) \
		-o $(BUILD_DIR)/spike_fuzz -ldl -lpthread

clean:
	rm -rf $(BUILD_DIR)

.DEFAULT_GOAL = $(FUZZ_TARGET)

.PHONY: clean spike
