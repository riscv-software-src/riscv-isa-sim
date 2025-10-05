#!/usr/bin/env bash

set -e -x

WORKDIR="$1"
CONFIGDIR="$2"
RESULTDIR="$WORKDIR"/snippy-tests

mkdir -p "$WORKDIR"
mkdir -p "$RESULTDIR"

generate_script=`git rev-parse --show-toplevel`/ci-tests/generate-snippy-test.sh

"$generate_script" "$CONFIGDIR"/basic.yaml "$RESULTDIR"/basic64.elf \
  "$CONFIGDIR"/boot-code.s riscv64-unknown-elf rv64i_zicsr lp64

"$generate_script" "$CONFIGDIR"/basic.yaml "$RESULTDIR"/basic32.elf \
  "$CONFIGDIR"/boot-code.s riscv32-unknown-elf rv32i_zicsr ilp32

"$generate_script" "$CONFIGDIR"/compressed.yaml "$RESULTDIR"/compressed64.elf \
  "$CONFIGDIR"/boot-code.s riscv64-unknown-elf rv64ic_zicsr lp64

"$generate_script" "$CONFIGDIR"/compressed.yaml "$RESULTDIR"/compressed32.elf \
  "$CONFIGDIR"/boot-code.s riscv32-unknown-elf rv32ic_zicsr ilp32

"$generate_script" "$CONFIGDIR"/double-fp.yaml "$RESULTDIR"/double64.elf \
  "$CONFIGDIR"/boot-code-f.s riscv64-unknown-elf rv64ifd_zicsr lp64d

"$generate_script" "$CONFIGDIR"/double-fp.yaml "$RESULTDIR"/double32.elf \
  "$CONFIGDIR"/boot-code-f.s riscv32-unknown-elf rv32ifd_zicsr ilp32d

"$generate_script" "$CONFIGDIR"/single-fp.yaml "$RESULTDIR"/float64.elf \
  "$CONFIGDIR"/boot-code-f.s riscv64-unknown-elf rv64if_zicsr lp64f

"$generate_script" "$CONFIGDIR"/single-fp.yaml "$RESULTDIR"/float32.elf \
  "$CONFIGDIR"/boot-code-f.s riscv32-unknown-elf rv32if_zicsr ilp32f

"$generate_script" "$CONFIGDIR"/vector.yaml "$RESULTDIR"/vector64.elf \
  "$CONFIGDIR"/boot-code-vf.s riscv64-unknown-elf rv64gcv lp64d
