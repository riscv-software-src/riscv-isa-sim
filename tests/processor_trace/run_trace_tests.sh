#!/usr/bin/env bash

# Script to test the processor instruction and data trace code.
# This is done by running a set of RISCV binaries through spike and checking
# against expected output files.
# The binary files are taken from the reference flow in the riscv-trace-spec repository
# https://github.com/riscv-non-isa/riscv-trace-spec/tree/main/referenceFlow/tests/test_files

set -e

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ $# -lt 1 ]; then
    echo "Usage: $0 path-to-spike-binary"
    exit 1
fi

spike_path=`realpath $1`
if [ ! -e $spike_path ]; then
    echo "Spike binary $spike_path not found"
    exit 1
else
    if [ ! -x $spike_path ]; then
        echo "Spike binary $spike_path is not executable"
        exit 1
    fi
fi

echo "Running tests using $spike_path"

data_dir=`realpath $script_dir/test_files`
memory="-m0x20010000:0x40000,0x80000000:0x400000"

errors_found=0

# Uncompress the expected file for the given trace type and return the name of the expected file
function get_expected() {
    name=$1
    trace_type=$2

    expected=$name.spike_${trace_type}_trace
    src=$data_dir/expected/$expected.gz
    if [ ! -e $src ]; then
        echo "Error: unable to find expected file $src"
        exit 1
    fi
    cp $src .
    gunzip $expected.gz
    echo $expected
}

# Run a test using a given binary
function run_test() {
    name=$1; shift
    args="$@"
    input=$data_dir/input/$name.riscv
    if [ ! -e $input ]; then
        echo "Error: unable to find input file $input"
        exit 1
    fi

    d_expected=`realpath $(get_expected $name data)`
    i_expected=`realpath $(get_expected $name pc)`
    d_output=`realpath $name.d_trace`
    i_output=`realpath $name.i_trace`

    # Run with both instruction and data trace enabled
    $spike_path $memory --inst-trace=$i_output --data-trace=$d_output $args $input > $name.log 2>&1

    # Check the output is as expected
    if diff $i_output $i_expected > /dev/null 2>&1; then
        echo "$name instruction trace (combined) PASSED"
    else
        echo "Difference in instruction trace for test $name"
        echo "tkdiff $i_output $i_expected"
        errors_found=$((errors_found+1))
    fi
    if diff $d_output $d_expected > /dev/null 2>&1; then
        echo "$name data trace (combined) PASSED"
    else
        echo "Difference in data trace for test $name"
        echo "tkdiff $d_output $d_expected"
        errors_found=$((errors_found+1))
    fi
    rm $i_output
    rm $d_output

    # Run with just instruction trace enabled
    $spike_path $memory --inst-trace=$i_output $args $input > $name.log 2>&1

    # Check the output is as expected
    if diff $i_output $i_expected > /dev/null 2>&1; then
        echo "$name instruction trace PASSED"
    else
        echo "Difference in instruction trace for test $name"
        echo "tkdiff $i_output $i_expected"
        errors_found=$((errors_found+1))
    fi

    # Run with just data trace enabled
    $spike_path $memory --data-trace=$d_output $args $input > $name.log 2>&1

    # Check the output is as expected
    if diff $d_output $d_expected > /dev/null 2>&1; then
        echo "$name data trace PASSED"
    else
        echo "Difference in data trace for test $name"
        echo "tkdiff $d_output $d_expected"
        errors_found=$((errors_found+1))
    fi
}

regression_dir=trace_regression
rm -fr $regression_dir
mkdir -p $regression_dir
pushd $regression_dir

# 64-bit binaries
tests="vvadd pmp br_j_asm mt-matmul"
for t in $tests; do
    echo "Run 64-bit test $t"
    run_test $t
done

# 32-bit binaries
tests="embench-statemate"
for t in $tests; do
    echo "Run 32-bit test $t"
    run_test $t --isa=RV32IMAFDC
done

popd

echo "Test output can be found in $regression_dir"
if [ $errors_found -ne 0 ]; then
    echo "Errors found in $errors_found tests"
fi
exit $errors_found
