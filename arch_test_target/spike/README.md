# Using the Spike Simulator as an Architectural test model

This is a reference for running Spike as a target for the RISC-V Architectural Test framework.

## Getting Spike

The Spike repository should be cloned from [here](https://github.com/riscv/riscv-isa-sim/), preferably at the same directory level as the riscv-compliance repository.

## Building Spike

The [README.md](../README.md) at the top level of the riscv-isa-sim directory gives details on building an executable spike model.

## Adding Spike as a target to the Architectural Test framework

Also at the top level is an ``arch_test_target directory``. This directory contains all the collaterals
required to add Spike as a target to the architectural test framework.

The user will have to modify the ``TARGETDIR`` variable in ``arch_test_target/spike/Makfile.include`` to point to the
absolute address of the ``riscv-isa-sim/arch_test_target``. 

The user can also modify the ``XLEN`` variable based on whether 32-bit or 64-bit tests need to be run.
If you would like to run tests of a single extension then set the `RISCV_DEVICE` to that extension
name (eg. M, C, Zifencei, etc). Leaving the ``RISCV_DEVICE`` empty would indicate running all tests
for all extensions available in the ``device/rv{XLEN}i_m`` directory No other variables should be modified.

Now clone the architectural test framework repo and copy the updated Makefile.include to it:

```
  $ git clone https://github.com/riscv/riscv-compliance.git
  $ cd riscv-compliance
  $ cp <custom-path>/riscv-isa-sim/arch_test_target/Makefile.include .
```

You can execute the tests from the root directory of the riscv-compliance repo:

```
make compile simulate verify
```

## Updating the target for new tests

As tests for new extensions are added to the architectural test repo, the spike target (i.e.
arch_test_target directory) will also need to be updated accordingly. Please refer to the [Porting a new target](https://github.com/riscv/riscv-compliance/blob/master/doc/README.adoc#5-porting-a-new-target)
section for more details on what those changes/updates should be.












