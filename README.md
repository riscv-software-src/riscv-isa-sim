RISC-V ISA Simulator
======================

Author  : Andrew Waterman, Yunsup Lee

Date    : June 19, 2011

Version : (under version control)

About
-------------

The RISC-V ISA Simulator implements a functional model of one or more
RISC-V processors.

Build Steps
---------------

We assume that the RISCV environment variable is set to the RISC-V tools
install path, and that the riscv-fesvr package is installed there.

    $ mkdir build
    $ cd build
    $ ../configure --prefix=$RISCV --with-fesvr=$RISCV
    $ make
    $ [sudo] make install


If you want to build spike with basic block vector generation support for 
simpoint creation, pass --enable-simpoint to configure:

    $ ../configure --prefix=$RISCV --with-fesvr=$RISCV --enable-simpoint
    $ make
    $ [sudo] make install

Compiling and Running a Simple C Program
-------------------------------------------

Install spike (see Build Steps), riscv-gnu-toolchain, and riscv-pk.

Write a short C program and name it hello.c.  Then, compile it into a RISC-V
ELF binary named hello:

    $ riscv64-unknown-elf-gcc -o hello hello.c

Now you can simulate the program atop the proxy kernel:

    $ spike pk hello

Simulating a New Instruction
------------------------------------

Adding an instruction to the simulator requires two steps:

  1.  Describe the instruction's functional behavior in the file
      riscv/insns/<new_instruction_name>.h.  Examine other instructions
      in that directory as a starting point.

  2.  Add the opcode and opcode mask to riscv/opcodes.h.  Alternatively,
      add it to the riscv-opcodes package, and it will do so for you:

         $ cd ../riscv-opcodes
         $ vi opcodes       // add a line for the new instruction
         $ make install

  3.  Rebuild the simulator.

Interactive Debug Mode
---------------------------

To invoke interactive debug mode, launch spike with -d:

    $ spike -d pk hello

To see the contents of a register (0 is for core 0):

    : reg 0 a0

To see the contents of a memory location (physical address in hex):

    : mem 2020

To see the contents of memory with a virtual address (0 for core 0):

    : mem 0 2020

You can advance by one instruction by pressing <enter>. You can also
execute until a desired equality is reached:

    : until pc 0 2020                   (stop when pc=2020)
    : until mem 2020 50a9907311096993   (stop when mem[2020]=50a9907311096993)

Alternatively, you can execute as long as an equality is true:

    : while mem 2020 50a9907311096993

You can continue execution indefinitely by:

    : r

At any point during execution (even without -d), you can enter the
interactive debug mode with `<control>-<c>`.

To end the simulation from the debug prompt, press `<control>-<c>` or:

    : q


Generating Basic Block Vector for Simpoint tool
-----------------------------------------------

If you compiled spike with basic block tracking support, running an executable in spike
will also generate a bbv_proc_0.bb.gz in the run directory. If you are running spike in 
multiprocessor mode, one basic block vector file will be created for each processor.
Unzip this file and use the Simpoint tool to obtain simpoints:

    $ gunzip bbv_proc_0.bb.gz
    $ <path to SimPoint.3.2>/bin/simpoint -loadFVFile bbv_proc_0.bb -maxK 10 -saveSimpoints <benchmark>.simpoints -saveSimpointWeights <benchmark>.weights

The above command will generate the simpoints and their corresponding weights for the benchmark of 
interest (whose basic block vector you created using Spike). For more information on Simpoint
visit http://cseweb.ucsd.edu/~calder/simpoint/ 

