# Spike as a Reference Model

The Spike RISC-V ISA simulator can be a golden reference model for CPU co-simulation as well.
This directory contains the wrapper files for using Spike as the REF used in DiffTest.

## Supported CPUs

RISC-V allows diverse implementation-specific behaviors as long as they don't violate the ISA.
Therefore, to co-simulate Spike, one of the RISC-V implementations, with another implementation
without errors, we have to align their behaviors on the undefined or design-specific regions.

Currently we are supporting CPUs including:
- [XiangShan](https://github.com/OpenXiangShan/XiangShan)
- [NutShell](https://github.com/OSCPU/NutShell)
- [Rocket Chip](https://github.com/chipsalliance/rocket-chip)

## How to Compile

To compile the Spike into a dynamic library for co-simulation via DiffTest, run:

```
cd riscv-isa-sim/difftest
make CPU=XIANGSHAN -jN
```

Replace `XIANGSHAN` with `ROCKET` or `NUTSHELL` for co-simulation with Rocket or NutShell.

## Coverage Instrumentation with LLVM

Add `SANCOV=1` to the `make` command to instrument the source code with LLVM coverage.
This enables fuzzers to be guided with branch coverage feedback from a golden RISC-V model.
