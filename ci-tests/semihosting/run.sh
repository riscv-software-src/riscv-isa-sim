#!/usr/bin/env bash
# Build semihosting_test.c and debug_console_test.c for RV32 and RV64 and run both on spike.
#
# Usage: [CC=compiler] run.sh [path/to/spike]

set -euo pipefail

HERE=`pwd`
CI="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
INSTALL=$HERE/install
RUN=$HERE/run

SPIKE="${1:-${SPIKE:-$INSTALL/bin/spike}}"
CC="${CC:-riscv64-unknown-elf-gcc}"

mkdir -p $RUN
cd $RUN

run() {
  local xlen=$1 arch=$2 abi=$3
  local cflags=(
    -O0 -g -march="$arch" -mabi="$abi" -Wall
    # for newlib
    --specs=semihost.specs
    # for picolibc
    # --specs=picolibc.specs --oslib=semihost --crt0=hosted -mcmodel=medany
  )
  local spike_flags=(
    --isa="$arch"
    --disable-dtb --no-dm "-m0:0x100000000,0xffffffffffff0000:0x10000"
    --semihosting
  )
  local fetch_fault_spike_flags=(
    --isa="$arch"
    --disable-dtb --no-dm -m0x1000:0x1000
    --semihosting
  )
  local test

  echo "== RV$xlen, $arch, $abi =="
  test=semihosting_test
  "$CC" "${cflags[@]}" -o "$test$xlen" "$CI/${test}.c"
  "$SPIKE" "${spike_flags[@]}" "$test$xlen" cmdline-one cmdline-two

  test=debug_console_test
  "$CC" "${cflags[@]}" -o "$test$xlen" "$CI/${test}.c"
  echo R | "$SPIKE" "${spike_flags[@]}" "$test$xlen" 2>&1 \
    | grep "Csemihosting WRITE0"

  test=ebreak_fetch_fault_test
  "$CC" -march="$arch" -mabi="$abi" -nostdlib -nostartfiles \
    -Wl,-T,"$CI/${test}.ld" -Wl,--build-id=none \
    -o "$test$xlen" "$CI/${test}.S"
  "$SPIKE" "${fetch_fault_spike_flags[@]}" "$test$xlen"
}

run 32 rv32imac_zicsr ilp32
run 64 rv64gc_zicsr lp64d

echo "ALL PASSED"
