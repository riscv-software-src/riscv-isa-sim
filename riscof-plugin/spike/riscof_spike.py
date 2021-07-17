import os
import re
import shutil
import subprocess
import shlex
import logging
import random
import string
from string import Template
import sys

import riscof.utils as utils
import riscof.constants as constants
from riscof.pluginTemplate import pluginTemplate

logger = logging.getLogger()

class spike(pluginTemplate):
    __model__ = "spike"

    #TODO: please update the below to indicate family, version, etc of your DUT.
    __version__ = "XXX"

    def __init__(self, *args, **kwargs):
        sclass = super().__init__(*args, **kwargs)

        config = kwargs.get('config')

        # capture the path of the spike executable either from the config.ini or
        # default to "spike"
        self.dut_exe = os.path.join(config['PATH'] if 'PATH' in config else "","spike")

        # capture the number of parallel jobs that can be used for running tests
        # on spike either from the config.ini or default to 1
        self.num_jobs = str(config['jobs'] if 'jobs' in config else 1)

        # The following captures the paths of the isa yaml, platform yaml and
        # the path of the plugin from the config.ini file. If the variables are 
        # missing in the config.ini then an error is thrown. 
        # We also capture the make command from the config.ini which could
        # default to using "make". Options for this command could be "pmake" as
        # well.
        if config is None:
            logger.error("Please enter input file paths in configuration.")
            raise SystemExit
        try:
            self.isa_spec = os.path.abspath(config['ispec'])
            self.platform_spec = os.path.abspath(config['pspec'])
            self.pluginpath = os.path.abspath(config['pluginpath'])
            self.make = config['make'] if 'make' in config else 'make'
        except KeyError as e:
            logger.error("Please check the spike section in config for missing values.")
            logger.error(e)
            raise SystemExit

        # logger statements to print on the terminal during the run
        logger.debug("SPIKE Parallel plugin initialised using the following configuration.")
        for entry in config:
            logger.debug(entry+' : '+config[entry])
        return sclass

    def initialise(self, suite, work_dir, archtest_env):
        self.work_dir = work_dir

        #TODO: The following assumes you are using the riscv-gcc toolchain. If
        #      not please change appropriately

        # Note the march is not hardwired here, because it will change for each
        # test.
        self.compile_cmd = 'riscv{1}-unknown-elf-gcc -march={0} \
         -static -mcmodel=medany -fvisibility=hidden -nostdlib -nostartfiles\
         -T '+self.pluginpath+'/env/link.ld\
         -I '+self.pluginpath+'/env/\
         -I ' + archtest_env

        # You can choose to add the objdump commands also by uncommenting the
        # following line and assigning the relevant command
        #self.objdump_cmd = ''

        # set all the necessary variables like compile command, elf2hex
        # commands, objdump cmds. etc whichever you feel necessary and required
        # for your plugin here

    def build(self, isa_yaml, platform_yaml):

        # The checked isa and platform yamls are provided by the riscof tool to
        # this function. The following loads the yaml as a python dictionary.
        ispec = utils.load_yaml(isa_yaml)['hart0']

        # we capture the max xlen value from the isa yaml.
        self.xlen = ('64' if 64 in ispec['supported_xlen'] else '32')
        
        #TODO: The following assumes you are using the riscv-gcc toolchain. If
        #      not please change appropriately and sets the mabi argument for
        #      compilation
        self.compile_cmd = self.compile_cmd+' -mabi='+('lp64 ' if 64 in ispec['supported_xlen'] else 'ilp32 ')

        # the following variable is used capture the value of the --isa argument
        # needed for spike.
        self.isa = 'rv' + self.xlen
        if "I" in ispec["ISA"]:
            self.isa += 'i'
        if "M" in ispec["ISA"]:
            self.isa += 'm'
        if "C" in ispec["ISA"]:
            self.isa += 'c'

        # the Folloing piece of code checks if the toolchain, spike and make
        # commands exist on the command line. If not, then an error is thrown.
        compiler = "riscv{0}-unknown-elf-gcc".format(self.xlen)
        if shutil.which(compiler) is None:
            logger.error(compiler+": executable not found. Please check environment setup.")
            raise SystemExit
        if shutil.which(self.dut_exe) is None:
            logger.error(self.dut_exe+ ": executable not found. Please check environment setup.")
            raise SystemExit
        if shutil.which(self.make) is None:
            logger.error(self.make+": executable not found. Please check environment setup.")
            raise SystemExit

        # based on the validated isa and platform configure your simulator or
        # build your RTL here

    def runTests(self, testList, cgf_file=None):

        # Here we use the makefile utility offered by riscof to quickly create a
        # Makefile where each target of the makefile corresponds to a single
        # test compilation and run.
        make = utils.makeUtil(makefilePath=os.path.join(self.work_dir, "Makefile." + self.name[:-1]))

        # set the jobs that can be used by the make command.
        make.makeCommand = self.make + ' -j' + self.num_jobs

        # we will not parse through each test selected by the framework and
        # create makefile targets.
        for file in testList:

            # capture the testentry from the test-list dictionary
            testentry = testList[file]

            # capture the path of the assembly test
            test = testentry['test_path']

            # capture the directory where artifacts of the compilation of the
            # test should be generated.
            test_dir = testentry['work_dir']

            # Spike could be used to capture coverage of the tests. This mode is
            # enabled when the covergroup files are supplied as an argument to
            # this function. 
            # Under normal dut mode, we name the compiled elf of the test as
            # "dut.elf"
            if cgf_file is not None:
                elf = 'ref.elf'
            else:
                elf = 'dut.elf'

            # We not start building the execution command for the test. First
            # let make change directories to the work_dir specified by the test.
            execute = "@cd "+testentry['work_dir']+";"

            # Set the march value, test name and elf name to the compile command. 
            # NOTE: here we are going to use riscv32- toolchain for RV32 systems
            # and riscv64- toolchain for RV64 systems. If you do not want this
            # distinction, you can replace "self.xlen" in the below line with a
            # hardcoded number under quotes: "64"
            cmd = self.compile_cmd.format(testentry['isa'].lower(), self.xlen) + ' ' + test + ' -o ' + elf

            #TODO: we are using -D to enable compile time macros. If your
            # toolchain is not riscv-gcc you may want to change the below code
            # The compile macros are available within the test-entry. 
            # Users can further append their own compile macros that may be
            # required for custom vendor specific-code or for assertions.
            compile_cmd = cmd + ' -D' + " -D".join(testentry['macros'])

            # append compile command to the makefile execution command.
            execute+=compile_cmd+";"

            # set name and path of the signature file. All dut signatures must
            # be named "DUT-<dut-name>.signature". This is what the RISCOF
            # framework will look for when matching signatures.
            sig_file = os.path.join(test_dir, self.name[:-1] + ".signature")

        
            # Under dut mode, the following basically creates the command for
            # running the elf on the spike executable and generate signature
            # from it.
            # Under coverage mode, we expext the commit log also to be generated
            # which can be fed into the isac tool for coverage collection.
            if cgf_file is not None:
                execute += self.dut_exe + ' --log-commits --log dump --isa={0} +signature={1} +signature-granularity=4 {2};'.format(self.isa, sig_file, elf)
            else:
                execute += self.dut_exe + ' --isa={0} +signature={1} +signature-granularity=4 {2};'.format(self.isa, sig_file, elf)

            # under coverage mode, we collect the coverage labels that a
            # particular test should be hitting.
            cov_str = ' '
            for label in testentry['coverage_labels']:
                cov_str+=' -l '+label

            # under coverage mode, we also append the isac commands for
            # collecting coverage from the test.
            if cgf_file is not None:
                coverage_cmd = 'riscv_isac --verbose error coverage -d \
                        -t dump --parser-name spike -o coverage.rpt  \
                        --sig-label begin_signature  end_signature \
                        --test-label rvtest_code_begin rvtest_code_end \
                        -e ref.elf -c ' + ' -c '.join(cgf_file) + cov_str + " -x "+ self.xlen +';'
            else:
                coverage_cmd = ''
            execute+=coverage_cmd

            # create a makefile target for the current test.
            make.add_target(execute)

        # run all the targets in parallel    
        make.execute_all(self.work_dir)
