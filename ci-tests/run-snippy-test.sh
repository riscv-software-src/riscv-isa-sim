#!/usr/bin/env bash

set -e -x
set -o pipefail

ROOT="$1"
NUMINSTRS="$2"
BOOTCODE="$3"
TRIPLE="$4"
ARCH="$5"
EXTENSIONS="$6"
ABI="$7"
SPIKE_PATH="$8"

CONFIGDIR="$ROOT"/ci-tests/snippy-tests

CONFIG="test-$ARCH-$ABI.yaml"

base=$(basename "$CONFIG" .yaml)
testfile="$base".elf
tmpelf="$base".tmp.elf
# exclude C_JR and C_JALR and some othe compressed opcodes as snippy has issues with them
# exclude EBREAK/ECALL as we want non-privileged instructions
# exclude lr.rl and sc.aq as they don't make sense
"$CONFIGDIR"/generate-snippy-test.sh --march "$ARCH" --mtriple "$TRIPLE" --extensions "$EXTENSIONS" --num-instrs $NUMINSTRS --ignore-opcode-regex "C_JR|C_JALR|EBREAK|ECALL|C_.*(SP|HINT|UNIMP).*|LR_.*_RL|SC_.*_AQ" > "$CONFIG"
llvm-snippy "$CONFIG" -o "$tmpelf" --seed 1 -riscv-disable-misaligned-access --layout-include-dir "$ROOT"/ci-tests/snippy-tests
riscv64-linux-gnu-gcc -O0 -march="$ARCH" -mabi="$ABI" -T "$tmpelf".ld -T "$CONFIGDIR"/linker-entry.ld "$tmpelf" "$BOOTCODE" -nostdlib -static -o "$testfile" -Wl,--build-id=none

error=0
if ! timeout --foreground 60s "$SPIKE_PATH" -l --log-commits --isa "$ARCH" "$testfile"
then
  echo "TIMEOUT: $testfile"
  error=1
else
  echo "SUCCESS: $testfile"
fi
exit $error
