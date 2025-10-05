#!/usr/bin/env bash

set -x

TESTDIR="$1"
SPIKE_PATH="$2"

error=0
for testfile in "$TESTDIR"/*; do
  bitness=$(basename "$testfile" .elf | grep -o "[0-9]\+")
  if ! timeout --foreground 5s "$SPIKE_PATH" -l --log-commits --isa rv"$bitness"ifdcv_zicsr "$testfile"
  then
    echo "TIMEOUT: $testfile"
    error=1
  else
    echo "SUCCESS: $testfile"
  fi
done
exit $error
