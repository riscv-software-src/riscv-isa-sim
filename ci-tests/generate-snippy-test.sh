#!/usr/bin/env bash

set -e -x

CONFIG="$1"
RESULT="$2"
BOOTCODE="$3"
TRIPLE="$4"
ARCH="$5"
ABI="$6"

CONFIGDIR=$(dirname "$CONFIG")

base=$(basename "$CONFIG" .yaml)
elf="$base".elf
./llvm-snippy "$CONFIG" -o "$elf" --seed 1 -mtriple="$TRIPLE" -march="$ARCH" -riscv-disable-misaligned-access
riscv64-linux-gnu-gcc -O0 -march="$ARCH" -mabi="$ABI" -T "$elf".ld -T "$CONFIGDIR"/linker-entry.ld "$elf" "$BOOTCODE" -nostdlib -static -o "$RESULT" -Wl,--build-id=none

