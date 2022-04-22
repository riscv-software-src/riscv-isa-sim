#!/usr/bin/env bash

#
#   Script to build all (19) the "embench-iot" test cases,
#   as 32-bit Elf binaries for RISC-V (specifically RV32),
#   to be run on the SPIKE instruction set simulator (ISS).
#
#   The resultant Elf files may be run under SPIKE using:
#
#   host% spike --isa=RV32IMAFDC -m0x20010000:0x40000,0x80000000:0x400000 embench-<file>.riscv
#
#   --------------------------------------------------------------------
#
#   host% git clone https://github.com/embench/embench-iot.git
#
#   --------------------------------------------------------------------
#

build_dir="build"

#
# list of paths ... the first found will be used
#
toolchain_paths=(
    "/opt/riscv-tools/rv32"
)


# find the first existing toolchain path
for path in "${toolchain_paths[@]}"
do
    if [[ -d "${path}/bin" ]] ; then
        export PATH="${path}/bin:${PATH}"
        break
    fi
done

########################################################################

# build up the set of options to pass to the script "build_all.py"
options=()
options+=("--clean")
options+=("--verbose")

# use our local build directory
options+=("--builddir=${build_dir}")

# choose the arch and board to use ...
# Currently, out of the box, "ri5cyverilator" is the only
# supported board for RV32, so just use it!
options+=("--arch=riscv32")
options+=("--board=ri5cyverilator")

# the compiler should only compile, and not link each source file
cflags=()
cflags+=("-c")
cflags+=("-g")
cflags+=("-O3")
#   cflags+=("-DSPIKE_VERBOSE") # uses spike HTIF (i.e. additional diagnostics)
options+=("--cflags=${cflags[*]}")

# generate a linker mapping file for each executable generated
# and override where some of the segments starts!
# also override the entry-point to the (new) spike specific one!
ldflags=()
ldflags+=("-Wl,-Map=ld.map")
ldflags+=("-Ttext=0x20010000")
ldflags+=("-Tdata=0x80000000")
ldflags+=("--entry=embench_spike_entry")
ldflags+=("-nostartfiles")
options+=("--ldflags=${ldflags[*]}")

# need the maths library for: "cubic", "nbody" and "st" test-benches
options+=("--user-libs=-lm")

# prepend each generated Elf binary with "embench-"
# postpend each generated Elf binary with ".riscv"
options+=("--ld-output-pattern=-o embench-{0}.riscv")

########################################################################

set -x

# build (but not run) them all ...
./build_all.py "${options[@]}"

# list all the new Elf executables ...
find "${build_dir}" -name 'embench-*.riscv' -ls
find "${build_dir}" -name 'embench-*.riscv' | wc -l
