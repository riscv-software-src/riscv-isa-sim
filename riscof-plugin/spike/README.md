# Riscof Plugin for SPIKE RISC-V ISA Simulator

This directory provides all the files and artifacts required for running Spike as a Target on the 
[RISCOF](https://github.com/riscv/riscof) framework.

## Getting Spike

The Spike repository should be cloned from [here](https://github.com/riscv/riscv-isa-sim/).

## Building Spike

The [README.md](../../README.md) at the top level of the riscv-isa-sim directory gives details on building an executable spike model.

## Configuring Spike Plugin Model

If you are interested in running spike under specific isa extensions (i.e. rv64im, rv32ic, etc) you
can do this by editing the `ISA` node in the `spike_isa.yaml`. The plugin uses this field to
build the value that should be given to spike's `--isa` argument. 

As new extensions are added, you will also need to update the `build` function in `riscof_spike.py`
to parse the correct `--isa` values.

NOTE: the riscv-config yaml currently is only used to control the `--isa` arguments for spike and
does not configure spike in any other way

## Plugin Files


``` 
  env                          # contains the model header files and linker script
  riscof_spike.py              # plugin file for spike. MUST be prefixed with "riscof_"
  spike_isa.yaml               # ISA YAML configuration file
  spike_platform.yaml          # PLATFORM YAML configuration file
  __init__.py                  # used expose the plugin path.
  README.md                    # This file
```

## Adding Spike Plugin target to RISCOF

RISCOF requires a `config.ini` file to detect various parameters of the run like - name and path 
of the dut and reference plugins, the isa spec files, etc. A template of this this file can be generated
automatically using the `riscof setup` command . Please see the [Generate
Templates](https://riscof.readthedocs.io/en/stable/plugins.html#generate-templates)
guide for mode details on the syntax of this file and how to modify/build custom plugins.


For spike you will need to edit the dut plugin sections of the `config.ini` file with the following:

```
# Name of the DUT Plugin. All entries are case sensitive. Including the name.
[spike]

#path to the directory where the plugin is present. (required)
pluginpath=/clone-path/riscv-isa-sim/riscof-plugin/spike

#path where the riscv-isa-sim binary is available. (optional)
#if this value is absent, it is assumed that the binaries are available in the $PATH by default
PATH=<path-to-binaries>

#path to the ISA config file. (required)
ispec=/clone-path/riscv-isa-sim/riscof-plugin/spike/spike_isa.yaml

#path to the Platform config file. (required)
pspec=/clone-path/riscv-isa-sim/riscof-plugin/spike/spike_platform.yaml

#number of jobs to spawn in parallel while running tests on spike (optional)
jobs=1

#the make command to use while executing the make file. Any of bmake,cmake,pmake,make etc. (optional)
#Default is make
make=make
```

- Export command
```
pwd=pwd;export PYTHONPATH=$pwd:$PYTHONPATH;
```

## About the Plugin

The spike plugin (`riscof_spike.py` file) present here has the following four functions:

- \_\_init\_\_: This is the base initialization function. This function will check if the config.ini
  section corresponding to `spike` for the following variables and set variables accordingly:

  - PATH: if set in the config.ini file, then the plugin will use this as the spike executable, else
    will assume `spike` is available by default in the $PATH
  - ispec: this points to the riscv-config isa yaml for spike (`spike_isa.yaml`). If not present in
    the config.ini then the plugin will throw an error
  - pspec: this points to the riscv-config platform yaml for spike (`spike_platform.yaml`). If not present in
    the config.ini then the plugin will throw an error
  - make: if set in config.ini then the plugin uses that command to execute the make file, else
    default to using the `make` command
  - jobs: if set in config.ini then the makefile is run with so many jobs in parallel, else defaults
    to 1

- initialize: This function basically sets up the compiler command for compiling tests. Currently
  the plugin defaults to using `riscv[64/32]-unknown-elf-gcc` compiler. If you are using an
  alternate toolchain or executable, you will need to edit this function.

- build: This function will deduce the value of the `--isa` argument required for spike.

- runTests: This function will first create a Makefile, where each target of the Makefile
  corresponds to compiling a specific test and then running it on spike. If this plugin is being
  used to collect coverage of a test via isac, then this function will also append the isac commands
  for each test in the corresponding Makefile target.

  Once the Makefile is created, this function will execute the makefile targets in parallel
  depending on the `jobs` value selected in the \_\_init\_\_ function.

This plugin also supports coverage collection using ISAC. The coverage reported might be
inaccurate due to the missing information in the logs generated by spike. 
See [here](https://riscv-isac.readthedocs.io/en/stable/python_plugins.html#writing-your-own-plugins) for more details.