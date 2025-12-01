#!/usr/bin/env bash

set -e -x

WORKDIR="$1"
CONFIGDIR="$2"
SPIKE_PATH="$3"
RESULTDIR="$WORKDIR"/snippy-tests

mkdir -p "$WORKDIR"
mkdir -p "$RESULTDIR"

ROOT=`git rev-parse --show-toplevel`
run_test_script="$ROOT"/ci-tests/run-snippy-test.sh

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv64 rv64i_zicsr_zifencei "i" lp64 "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv32 rv32i_zicsr_zifencei "i" ilp32 "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv64 rv64ic_zicsr_zifencei "c - d" lp64 "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv32 rv32ic_zicsr_zifencei "c - d" ilp32 "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifd_zicsr_zifencei "d - c - zfa - zvfh" lp64d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifd_zicsr_zifencei "d - c - zfa - zvfh" ilp32d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64if_zicsr_zifencei "f - c" lp64f "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32if_zicsr_zifencei "f - c" ilp32f "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifc_zicsr_zifencei "f" lp64f "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifc_zicsr_zifencei "f" ilp32f "$SPIKE_PATH"

"$run_test_script" "$ROOT" 2000 \
  "$CONFIGDIR"/boot-code-vf.s riscv64 rv64gcv_zfa_zvfh "v" lp64d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifc_zicsr_zifencei_zfhmin "f + zfhmin - d" lp64f "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifc_zicsr_zifencei_zfhmin "f + zfhmin - d" ilp32f "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifdc_zicsr_zifencei "d - zfa - zvfh" lp64d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifdc_zicsr_zifencei "d - zfa - zvfh" ilp32d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifdc_zicsr_zifencei_zfhmin "d - zfa + zfhmin" lp64d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifdc_zicsr_zifencei_zfhmin "d - zfa + zfhmin" ilp32d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifdc_zicsr_zifencei_zfh_zfa "d + zfh" lp64d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifdc_zicsr_zifencei_zfh_zfa "d + zfh" ilp32d "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv64 rv64i_zicsr_zifencei_zca "zca" lp64 "$SPIKE_PATH"

"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv32 rv32i_zicsr_zifencei_zca "zca" ilp32 "$SPIKE_PATH"

# rv32-only zcf
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32if_zicsr_zifencei_zca_zcf "zcf" ilp32f "$SPIKE_PATH"
# zcd
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv64 rv64ifd_zicsr_zifencei_zca_zcd "zca + zcd - zfa - zfh" lp64d "$SPIKE_PATH"
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code-f.s riscv32 rv32ifd_zicsr_zifencei_zca_zcd "zca + zcd - zfa - zfh" ilp32d "$SPIKE_PATH"
# zcb
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv64 rv64i_zicsr_zifencei_zca_zcb_zmmul_zba_zbb "zca + zcb" lp64 "$SPIKE_PATH"
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv32 rv32i_zicsr_zifencei_zca_zcb_zmmul_zba_zbb "zca + zcb" ilp32 "$SPIKE_PATH"
# zawrs
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv64 rv64ia_zicsr_zifencei_zawrs "zawrs + zalrsc" lp64 "$SPIKE_PATH"
"$run_test_script" "$ROOT" 3000 \
  "$CONFIGDIR"/boot-code.s riscv32 rv32ia_zicsr_zifencei_zawrs "zawrs + zalrsc" ilp32 "$SPIKE_PATH"

