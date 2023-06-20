BUILD_DIR = $(abspath ./build)

CC  = clang
CXX = clang++

AFL_CC  ?= $(CC)
AFL_CXX ?= $(CXX)

# build spike
SPIKE_PATH      = $(abspath ..)
SPIKE_CONFIGURE = $(SPIKE_PATH)/configure

SPIKE_CFLAGS    = -O3 -g0 -DSPIKE_FUZZ
SPIKE_CXXFLAGS  = -O3 -Wno-c99-designator -g0 -DSPIKE_FUZZ
SPIKE_LDFLAGS   =

SPIKE_CONFIGURE_FLAGS = -q CC=$(CC) CXX=$(CXX) CFLAGS="$(SPIKE_CFLAGS)" \
	CXXFLAGS="$(SPIKE_CXXFLAGS)" LDFLAGS="$(SPIKE_LDFLAGS)"             \
	--with-boost=no --with-boost-asio=no --with-boost-regex=no
SPIKE_MAKEFILE = $(BUILD_DIR)/Makefile

# spike_fuzz sources
FUZZ_SRCS  = $(shell find . -maxdepth 1 -name '*.cc')
FUZZ_OBJS  = $(addprefix $(BUILD_DIR)/, $(patsubst %.cc, %.o, $(FUZZ_SRCS)))
CXX_FLAGS  = -O3 --std=c++20 -fPIC  -DSPIKE_FUZZ

ifneq ($(SPIKE_DEBUG),)
CXX_FLAGS += -DLOG_PATH=\"$(BUILD_DIR)/spike.log\"
endif

SPIKE_INC  = fesvr riscv disasm customext fdt softfloat spike_main spike_dasm
CXX_FLAGS += -I$(SPIKE_PATH) $(addprefix -I$(SPIKE_PATH)/, $(SPIKE_INC))
CXX_FLAGS += -I$(BUILD_DIR)

# spike_fuzz elf dependencies
SPIKE_LIB   = libriscv.a libdisasm.a libsoftfloat.a libfesvr.a libfdt.a
SPIKE_LINK  = $(addprefix $(BUILD_DIR)/, $(SPIKE_LIB))


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SPIKE_MAKEFILE): | $(BUILD_DIR)
	cd $(BUILD_DIR) && $(SPIKE_CONFIGURE) $(SPIKE_CONFIGURE_FLAGS)

spike: $(SPIKE_MAKEFILE)
	$(MAKE) -s -C $(BUILD_DIR) CC=$(AFL_CC) CXX=$(AFL_CXX)

$(BUILD_DIR)/%.o: %.cc spike
	$(AFL_CXX) $(CXX_FLAGS) $(INC_PATH) $< -c -o $@

libfuzz: spike $(FUZZ_OBJS)

fuzz: libfuzz
	$(AFL_CXX) $(FUZZ_OBJS) $(SPIKE_LINK) -o $(BUILD_DIR)/spike_fuzz

dummy: libfuzz spike_fuzz.c
	$(CC) -c spike_fuzz.c -o $(BUILD_DIR)/spike_fuzz.o
	$(CXX) $(BUILD_DIR)/spike_fuzz.o $(FUZZ_OBJS) $(SPIKE_LINK) \
		-o $(BUILD_DIR)/spike_fuzz -ldl -lpthread

clean:
	rm -rf $(BUILD_DIR)

.DEFAULT_GOAL = dummy

.PHONY: clean spike fuzz dummy libfuzz
