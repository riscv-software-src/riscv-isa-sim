CC  = clang
CXX = clang++

GUEST_ISA = riscv64
BUILD_DIR = $(abspath ./build)
TARGET    = $(BUILD_DIR)/$(GUEST_ISA)-spike-so

# Dependencies for building Spike
SPIKE_PATH     = $(abspath ..)
SPIKE_TARGET   = $(BUILD_DIR)/spike
SPIKE_MAKEFILE = $(BUILD_DIR)/Makefile

SPIKE_CFLAGS    = -O3 -DDIFFTEST
SPIKE_CXXFLAGS  = -O3 -DDIFFTEST -Wno-c99-designator
SPIKE_LDFLAGS   =

ifneq ($(CPU),)
SPIKE_CFLAGS   += -DCPU_$(CPU)
SPIKE_CXXFLAGS += -DCPU_$(CPU)
endif

ifeq ($(DEBUG),)
SPIKE_CFLAGS   += -g0
SPIKE_CXXFLAGS += -g0
endif

CONFIGURE_FLAGS = -q CC=$(CC) CXX=$(CXX)         \
    --with-boost=no	--with-boost-asio=no --with-boost-regex=no \
    CFLAGS="$(SPIKE_CFLAGS)" CXXFLAGS="$(SPIKE_CXXFLAGS)"      \
    LDFLAGS="$(SPIKE_LDFLAGS)"

SPIKE_INCS = fesvr riscv disasm customext fdt softfloat spike_main spike_dasm
INC_PATH   = -I$(SPIKE_PATH) -I$(BUILD_DIR) $(addprefix -I$(SPIKE_PATH)/, $(SPIKE_INCS))

SPIKE_SRCS = $(shell find $(SPIKE_PATH) -maxdepth 2 -name '*.cc') \
             $(shell find $(SPIKE_PATH) -maxdepth 2 -name '*.h')
SPIKE_LIBS = libriscv.a libdisasm.a libsoftfloat.a libfesvr.a libfdt.a
INC_LIBS   = $(addprefix $(BUILD_DIR)/, $(SPIKE_LIBS))

# We need some utilities from spike
CXX_UTILS = $(BUILD_DIR)/difftest_utils.cc

$(CXX_UTILS): $(SPIKE_PATH)/spike_main/spike.cc
	cp $< $(CXX_UTILS)
	@sed -i 's/static //g' $@
	@sed -i 's/int main/static int spike_main/g' $@

# Dependencies for building the difftest dynamic library
DIFFTEST_SOURCES = $(shell find . -maxdepth 1 -name '*.cc') $(CXX_UTILS)
DIFFTEST_HEADERS = $(shell find . -maxdepth 1 -name '*.h')
CXXFLAGS = $(SPIKE_CXXFLAGS) --std=c++17 -shared -fPIC -flto

ifneq ($(DEBUG),)
CONFIGURE_FLAGS += --enable-commitlog
CXXFLAGS += -DDIFFTEST_LOG_FILE=\"$(NOOP_HOME)/build/spike.log\"
endif

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SPIKE_MAKEFILE): $(SPIKE_PATH)/configure | $(BUILD_DIR)
	cd $(@D) && $(abspath $(SPIKE_PATH))/configure $(CONFIGURE_FLAGS)

$(SPIKE_TARGET): $(SPIKE_MAKEFILE) $(SPIKE_SRCS)
	$(MAKE) -s -C $(BUILD_DIR)

$(TARGET): $(SPIKE_TARGET) $(DIFFTEST_SOURCES) $(DIFFTEST_HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_PATH) $(DIFFTEST_SOURCES) $(INC_LIBS) -o $@

clean:
	rm -rf $(BUILD_DIR)

.DEFAULT_GOAL = $(TARGET)

.PHONY: clean
